#include "script/env.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/assert.hpp"
#include "script/expose/expose_base.hpp"
#include "script/expose/expose_graphics.hpp"
#include "script/expose/expose_network.hpp"
#include "script/util.hpp"
#include <alflib/file/file_io.hpp>
#include <dlog.hpp>

// ========================================================================== //
// Macros
// ========================================================================== //

#define DIB_SCRIPT_ENV_CHECK_ERROR(error)                                      \
  if (error != JsNoError) {                                                    \
    return Result::kUnknownError;                                              \
  }

// ========================================================================== //
// Environment Implementation
// ========================================================================== //

namespace dib {
namespace script {

Environment::Environment()
  : mRuntime(JS_INVALID_REFERENCE)
  , mContext(JS_INVALID_REFERENCE)
  , mRootModule(nullptr)
{
  // Create runtime
  JsRuntimeAttributes runtimeAttributes =
    (JsRuntimeAttributes)(JsRuntimeAttributeEnableExperimentalFeatures |
                          JsRuntimeAttributeEnableIdleProcessing);
  JsErrorCode error = JsCreateRuntime(runtimeAttributes, nullptr, &mRuntime);
  DIB_ASSERT(error == JsNoError, "Failed to create ChakraCore runtime");

  // Create context
  error = JsCreateContext(mRuntime, &mContext);
  DIB_ASSERT(error == JsNoError, "Failed to create ChakraCore Context");
  error = JsSetCurrentContext(mContext);
  DIB_ASSERT(error == JsNoError, "Failed to set current ChakraCore Context");

  // Setup root module
  mRootModule = new Module;
  mRootModule->environment = this;
  mRootModule->path = alflib::Path{ ":env-root:" };
  mRootModule->isCompiled = false;
  error = JsInitializeModuleRecord(nullptr, nullptr, &mRootModule->record);
  DIB_ASSERT(error == JsNoError, "Failed to create root module");
  error = JsSetModuleHostInfo(mRootModule->record,
                              JsModuleHostInfo_FetchImportedModuleCallback,
                              (void*)OnFetchImportedModule);
  DIB_ASSERT(error == JsNoError, "Failed to set root module callback");
  error =
    JsSetModuleHostInfo(mRootModule->record,
                        JsModuleHostInfo_FetchImportedModuleFromScriptCallback,
                        (void*)OnFetchImportedModuleFromScript);
  DIB_ASSERT(error == JsNoError, "Failed to set root module callback");
  error = JsSetModuleHostInfo(mRootModule->record,
                              JsModuleHostInfo_NotifyModuleReadyCallback,
                              (void*)OnNotifyModuleReady);
  DIB_ASSERT(error == JsNoError, "Failed to set root module callback");

  // Store module pointer in record
  error = JsSetModuleHostInfo(
    mRootModule->record, JsModuleHostInfo_HostDefined, (void*)mRootModule);
  DIB_ASSERT(error == JsNoError, "Failed to set root module data");

  // Create private storage
  mPrivateStorage = CreateObject();
  JsAddRef(mPrivateStorage, nullptr);
  SetProperty(GetGlobal(), PRIVATE_STORAGE_KEY, mPrivateStorage);

  // Expose functionality to scripts
  ExposeLogging();
  ExposeGraphics(*this);
  ExposeNetwork(*this);
}

// -------------------------------------------------------------------------- //

Environment::~Environment()
{
  // Destroy modules
  for (auto& entry : mModules) {
    JsRelease(entry.second->record, nullptr);
    delete entry.second;
  }
  delete mRootModule;

  JsRelease(mPrivateStorage, nullptr);

  // Dispose of runtime
  JsDisposeRuntime(mRuntime);
}

// -------------------------------------------------------------------------- //

Result
Environment::RunScript(const String& script, JsValueRef& output, void* userData)
{
  // Run script
  JsValueRef source = CreateString(script);
  JsValueRef url = CreateString(":script:");
  JsErrorCode error = JsRun(source,
                            (JsSourceContext)userData,
                            url,
                            JsParseScriptAttributeNone,
                            &output);
  if (error == JsErrorScriptException) {
    return Result::kException;
  } else if (error != JsNoError) {
    return Result::kUnknownError;
  }
  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

Result
Environment::RunFile(const alflib::Path& path,
                     JsValueRef& output,
                     void* userData)
{
  // Load the source
  alflib::FileIO io(path);
  String source = "";
  alflib::FileResult result = io.Open(alflib::FileIO::Flag::kRead |
                                      alflib::FileIO::Flag::kShareReadWrite);
  if (result != alflib::FileResult::kSuccess) {
    return Result::kUnknownError;
  }
  result = io.Read(source);
  if (result != alflib::FileResult::kSuccess) {
    return Result::kUnknownError;
  }

  // Run source
  return RunScript(source, output, userData);
}

// -------------------------------------------------------------------------- //

Environment::Module*
Environment::LoadModule(const alflib::Path& path)
{
  bool isNew;
  Module* module = GetModule(path, nullptr, isNew);
  if (isNew) {
    // Load the source
    alflib::FileIO io(path);
    String source = "";
    const alflib::FileResult result = io.Open(
      alflib::FileIO::Flag::kRead | alflib::FileIO::Flag::kShareReadWrite);
    if (result != alflib::FileResult::kSuccess) {
      DLOG_ERROR("Failed to read script file ({})", path.GetPathString());
      return nullptr;
    }
    io.Read(source);

    // Parse module
    JsValueRef exception;
    JsErrorCode error = JsParseModuleSource(module->record,
                                            (JsSourceContext)module,
                                            (BYTE*)source.GetUTF8(),
                                            source.GetSize(),
                                            JsParseModuleSourceFlags_DataIsUTF8,
                                            &exception);
    HandleException(error);
    HandleCompilationError(module, exception);
    if (error != JsNoError) {
      return nullptr;
    }
    module->isCompiled = true;
  }

  // Update to run all pending tasks
  Update();

  // Return the module
  return module;
}

// -------------------------------------------------------------------------- //

void
Environment::StorePrivate(JsValueRef object, const String& key)
{
  SetProperty(mPrivateStorage, key, object);
}

// -------------------------------------------------------------------------- //

JsValueRef
Environment::LoadPrivate(const String& key)
{
  return GetProperty(mPrivateStorage, key);
}

// -------------------------------------------------------------------------- //

JsValueRef
Environment::InstantiateObject(JsValueRef constructor)
{
  // Store the constructor in global
  JsValueRef global;
  JsGetGlobalObject(&global);
  SetProperty(global, "__diabas_constr", constructor);

  // Run script
  JsValueRef output;
  Result result = RunScript("new __diabas_constr();", output);
  if (result != Result::kSuccess) {
    return JS_INVALID_REFERENCE;
  }
  return output;
}

// -------------------------------------------------------------------------- //

JsValueRef
Environment::GetClassPrototype(Environment::Module* module,
                               const String& className)
{
  // Retrieve module namespace
  JsValueRef moduleNamespace;
  JsErrorCode error = JsGetModuleNamespace(module->record, &moduleNamespace);
  DIB_ASSERT(error == JsNoError, "Failed to retrieve module namespace");

  // Retrieve class
  JsPropertyIdRef clsId;
  JsCreatePropertyId(className.GetUTF8(), className.GetSize(), &clsId);
  JsValueRef cls;
  error = JsGetProperty(moduleNamespace, clsId, &cls);
  if (error != JsNoError) {
    return JS_INVALID_REFERENCE;
  }

  // Retrieve prototype
  JsPropertyIdRef prototypeId;
  JsCreatePropertyId("prototype", strlen("prototype"), &prototypeId);
  JsValueRef prototype;
  error = JsGetProperty(cls, prototypeId, &prototype);
  DIB_ASSERT(error == JsNoError,
             "Class is not correctly setyp, missing 'prototype' property");
  return prototype;
}

// -------------------------------------------------------------------------- //

JsValueRef
Environment::GetModuleNamespace(Environment::Module* module)
{
  // Retrieve module namespace
  JsValueRef moduleNamespace;
  JsErrorCode error = JsGetModuleNamespace(module->record, &moduleNamespace);
  DIB_ASSERT(error == JsNoError, "Failed to retrieve module namespace");
  return moduleNamespace;
}

// -------------------------------------------------------------------------- //
void
Environment::Update()
{
  // Run load tasks
  while (!mLoadTasks.empty()) {
    LoadTask task = mLoadTasks.front();
    mLoadTasks.pop();

    JsValueRef exception;
    JsErrorCode error = JsParseModuleSource(task.module->record,
                                            (JsSourceContext)task.module,
                                            (BYTE*)task.source.GetUTF8(),
                                            task.source.GetSize(),
                                            JsParseModuleSourceFlags_DataIsUTF8,
                                            &exception);
    HandleCompilationError(task.module, exception);
    if (error != JsNoError) {
      // TODO(Filip Björklund): Return error to user
      // DIB_ASSERT(false, "Failed to execute load task");
      continue;
    }
    task.module->isCompiled = true;
  }

  // Run execution tasks
  while (!mRunTasks.empty()) {
    RunTask task = mRunTasks.front();
    mRunTasks.pop();

    if (!task.module->isCompiled) {
      continue;
    }

    JsValueRef output;
    JsErrorCode error = JsModuleEvaluation(task.module->record, &output);
    HandleException(error);
    if (error != JsNoError) {
      // TODO(Filip Björklund): Return error to user
      // DIB_ASSERT(false, "Failed to execute run task");
      continue;
    }
  }
}

// -------------------------------------------------------------------------- //

Environment::Module*
Environment::GetModule(const alflib::Path& path, Module* parent, bool& isNew)
{
  // Check if module already exists
  auto moduleFind = mModules.find(path.GetCanonical().GetPathString());
  if (moduleFind != mModules.end()) {
    isNew = false;
    return moduleFind->second;
  }

  // Otherwise load module
  Module* module = new Module;
  module->environment = this;
  module->path = path;
  module->isCompiled = false;
  JsValueRef specifier = CreateString(path.GetAbsolutePath().GetPathString());
  JsInitializeModuleRecord(
    parent ? parent->record : nullptr, specifier, &module->record);
  JsAddRef(module->record, nullptr);
  DIB_ASSERT(module->record != nullptr,
             "Module record is null after initialization");

  // Setup module callbacks
  JsErrorCode error =
    JsSetModuleHostInfo(module->record,
                        JsModuleHostInfo_FetchImportedModuleCallback,
                        (void*)OnFetchImportedModule);
  DIB_ASSERT(error == JsNoError, "Failed to set module callback");
  error =
    JsSetModuleHostInfo(module->record,
                        JsModuleHostInfo_FetchImportedModuleFromScriptCallback,
                        (void*)OnFetchImportedModuleFromScript);
  DIB_ASSERT(error == JsNoError, "Failed to set module callback");
  error = JsSetModuleHostInfo(module->record,
                              JsModuleHostInfo_NotifyModuleReadyCallback,
                              (void*)OnNotifyModuleReady);
  DIB_ASSERT(error == JsNoError, "Failed to set module callback");

  // Store module pointer in record
  error = JsSetModuleHostInfo(
    module->record, JsModuleHostInfo_HostDefined, (void*)module);
  DIB_ASSERT(error == JsNoError, "Failed to set module data");

  // Add module and return it
  mModules[path.GetCanonical().GetPathString()] = module;
  isNew = true;
  return module;
}

// -------------------------------------------------------------------------- //

void
Environment::HandleCompilationError(Module* module, JsValueRef exception)
{
  // Return immediately if there were no problem
  if (exception == JS_INVALID_REFERENCE) {
    return;
  }

  // Retrieve message
  String description = GetPropertyString(exception, "description");
  String source = GetPropertyString(exception, "source");
  s32 line = GetPropertyInt(exception, "line");
  s32 column = GetPropertyInt(exception, "column");

  // Log message
  DLOG_ERROR("Failed to compile script {}:{}:{}: {}",
             module->path.GetPathString(),
             line,
             column,
             description);
}

// -------------------------------------------------------------------------- //

JsErrorCode
Environment::OnFetchImportedModule(
  [[maybe_unused]] JsModuleRecord referencingModuleRecord,
  [[maybe_unused]] JsValueRef specifier,
  [[maybe_unused]] JsModuleRecord* dependentModule)
{
  // Retrieve module
  Environment::Module* referencingModule = nullptr;
  JsGetModuleHostInfo(referencingModuleRecord,
                      JsModuleHostInfo_HostDefined,
                      (void**)&referencingModule);
  DIB_ASSERT(referencingModule != nullptr,
             "Module pointer has not been setup correctly");

  // Retrieve environment
  Environment* environment = referencingModule->environment;

  // Retrieve path to module
  alflib::File file =
    alflib::File(referencingModule->path).Sibling(GetString(specifier));

  // Get module record
  bool isNew;
  Environment::Module* referencedModule = environment->GetModule(
    file.GetPath().GetCanonical(), referencingModule, isNew);

  // Load the source
  alflib::FileIO io(file);
  String source = "";
  const alflib::FileResult result = io.Open(
    alflib::FileIO::Flag::kRead | alflib::FileIO::Flag::kShareReadWrite);
  if (result != alflib::FileResult::kSuccess) {
    return JsErrorInvalidArgument;
  }
  io.Read(source);

  // Create a load task for module
  Environment::LoadTask loadTask;
  loadTask.source = source;
  loadTask.module = referencedModule;
  environment->mLoadTasks.push(loadTask);

  // TEMP
  *dependentModule = referencedModule->record;
  return JsNoError;
};

// -------------------------------------------------------------------------- //

JsErrorCode
Environment::OnFetchImportedModuleFromScript(
  [[maybe_unused]] JsSourceContext referencingSourceContext,
  [[maybe_unused]] JsValueRef specifier,
  [[maybe_unused]] JsModuleRecord* dependentModule)
{
  return OnFetchImportedModule(nullptr, specifier, dependentModule);
};

// -------------------------------------------------------------------------- //

JsErrorCode
Environment::OnNotifyModuleReady(
  [[maybe_unused]] JsModuleRecord referencingModuleRecord,
  [[maybe_unused]] JsValueRef exceptionVar)
{
  // Retrieve module
  Environment::Module* referencingModule = nullptr;
  JsGetModuleHostInfo(referencingModuleRecord,
                      JsModuleHostInfo_HostDefined,
                      (void**)&referencingModule);
  DIB_ASSERT(referencingModule != nullptr,
             "Module pointer has not been setup correctly");

  // Retrieve environment
  Environment* environment = referencingModule->environment;

  // Add run task
  RunTask task{};
  task.module = referencingModule;
  environment->mRunTasks.push(task);
  return JsNoError;
}

}
}