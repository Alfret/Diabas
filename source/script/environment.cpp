// MIT License
//
// Copyright (c) 2019 Filip Björklund
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "script/environment.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/assert.hpp"
#include "dlog.hpp"
#include "script/module.hpp"

// ========================================================================== //
// Exposed Functions
// ========================================================================== //

namespace dib {
namespace script {

/** Logs a verbose message **/
JsValueRef CHAKRA_CALLBACK
ScriptLogVerboseFunction(JsValueRef callee,
                         bool isConstructCall,
                         JsValueRef* arguments,
                         u16 argumentCount,
                         void* callbackState)
{
  DIB_ASSERT(!isConstructCall, "Function is not a destructor call");

  // Output string
  String output = "";

  // Combine all arguments
  for (u16 i = 1; i < argumentCount; i++) {
    JsValueType type;
    JsGetValueType(arguments[i], &type);

    if (type == JsString) {
      size_t bufferSize = 0;
      if (JsCopyString(arguments[i], nullptr, 0, &bufferSize) == JsNoError) {
        char8* buffer = new char8[bufferSize + 1];
        buffer[bufferSize] = 0;
        JsCopyString(arguments[i], buffer, bufferSize, &bufferSize);
        output += buffer;
        delete[] buffer;
      }
    } else if (type == JsNumber) {
      f64 doubleValue;
      JsNumberToDouble(arguments[i], &doubleValue);
      output += String::ToString(doubleValue);
    } else if (type == JsBoolean) {
      bool boolValue;
      JsBooleanToBool(arguments[i], &boolValue);
      output += boolValue ? "true" : "false";
    }
  }

  // Log
  DLOG_VERBOSE(output.GetUTF8());
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

/** Logs an info message **/
JsValueRef CHAKRA_CALLBACK
ScriptLogInfoFunction(JsValueRef callee,
                      bool isConstructCall,
                      JsValueRef* arguments,
                      u16 argumentCount,
                      void* callbackState)
{
  DIB_ASSERT(!isConstructCall, "Function is not a destructor call");

  // Output string
  String output = "";

  // Combine all arguments
  for (u16 i = 1; i < argumentCount; i++) {
    JsValueType type;
    JsGetValueType(arguments[i], &type);

    if (type == JsString) {
      size_t bufferSize = 0;
      if (JsCopyString(arguments[i], nullptr, 0, &bufferSize) == JsNoError) {
        char8* buffer = new char8[bufferSize + 1];
        buffer[bufferSize] = 0;
        JsCopyString(arguments[i], buffer, bufferSize, &bufferSize);
        output += buffer;
        delete[] buffer;
      }
    } else if (type == JsNumber) {
      f64 doubleValue;
      JsNumberToDouble(arguments[i], &doubleValue);
      output += String::ToString(doubleValue);
    } else if (type == JsBoolean) {
      bool boolValue;
      JsBooleanToBool(arguments[i], &boolValue);
      output += boolValue ? "true" : "false";
    }
  }

  // Log
  DLOG_INFO(output.GetUTF8());
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

/** Logs an info message **/
JsValueRef CHAKRA_CALLBACK
ScriptLogWarningFunction(JsValueRef callee,
                         bool isConstructCall,
                         JsValueRef* arguments,
                         u16 argumentCount,
                         void* callbackState)
{
  DIB_ASSERT(!isConstructCall, "Function is not a destructor call");

  // Output string
  String output = "";

  // Combine all arguments
  for (u16 i = 1; i < argumentCount; i++) {
    JsValueType type;
    JsGetValueType(arguments[i], &type);

    if (type == JsString) {
      size_t bufferSize = 0;
      if (JsCopyString(arguments[i], nullptr, 0, &bufferSize) == JsNoError) {
        char8* buffer = new char8[bufferSize + 1];
        buffer[bufferSize] = 0;
        JsCopyString(arguments[i], buffer, bufferSize, &bufferSize);
        output += buffer;
        delete[] buffer;
      }
    } else if (type == JsNumber) {
      f64 doubleValue;
      JsNumberToDouble(arguments[i], &doubleValue);
      output += String::ToString(doubleValue);
    } else if (type == JsBoolean) {
      bool boolValue;
      JsBooleanToBool(arguments[i], &boolValue);
      output += boolValue ? "true" : "false";
    }
  }

  // Log
  DLOG_WARNING(output.GetUTF8());
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

/** Logs an info message **/
JsValueRef CHAKRA_CALLBACK
ScriptLogErrorFunction(JsValueRef callee,
                       bool isConstructCall,
                       JsValueRef* arguments,
                       u16 argumentCount,
                       void* callbackState)
{
  DIB_ASSERT(!isConstructCall, "Function is not a destructor call");

  // Output string
  String output = "";

  // Combine all arguments
  for (u16 i = 1; i < argumentCount; i++) {
    JsValueType type;
    JsGetValueType(arguments[i], &type);

    if (type == JsString) {
      size_t bufferSize = 0;
      if (JsCopyString(arguments[i], nullptr, 0, &bufferSize) == JsNoError) {
        char8* buffer = new char8[bufferSize + 1];
        buffer[bufferSize] = 0;
        JsCopyString(arguments[i], buffer, bufferSize, &bufferSize);
        output += buffer;
        delete[] buffer;
      }
    } else if (type == JsNumber) {
      f64 doubleValue;
      JsNumberToDouble(arguments[i], &doubleValue);
      output += String::ToString(doubleValue);
    } else if (type == JsBoolean) {
      bool boolValue;
      JsBooleanToBool(arguments[i], &boolValue);
      output += boolValue ? "true" : "false";
    }
  }

  // Log
  DLOG_ERROR(output.GetUTF8());
  return JS_INVALID_REFERENCE;
}

}
}

// ========================================================================== //
// Private Functions
// ========================================================================== //

namespace dib {
namespace script {

/** Callback for fetching imported module **/
static JsErrorCode
OnEnvFetchImported(JsModuleRecord referencingModule,
                   JsValueRef specifier,
                   JsModuleRecord* dependentModule)
{
  // Retrieve referencing module
  Module* module;
  JsGetModuleHostInfo(referencingModule,
                      JsModuleHostInfo_HostDefined,
                      reinterpret_cast<void**>(&module));
  DIB_ASSERT(
    module != nullptr,
    "Failed to fetch imported module. Referencing module is not valid");

  // Retrieve specifier
  size_t size = 0;
  JsCopyString(specifier, nullptr, 0, &size);
  char8* string = new char8[size + 1];
  string[size] = 0;
  JsCopyString(specifier, string, size, &size);

  // Load module
  alflib::File siblingFile = module->GetFile().Sibling(string);
  Module* loadedModule = module->GetEnvironment().LoadModule(
    siblingFile.GetPath(), referencingModule);
  DIB_ASSERT(loadedModule != nullptr, "Failed to load dependant module");

  // Set loaded module
  *dependentModule = loadedModule->GetHandle();

  // TEMP
  return JsNoError;
};

// Callback for fetching imported from script
static JsErrorCode
OnEnvFetchImportedFromScript(JsSourceContext referencingSourceContext,
                             JsValueRef specifier,
                             JsModuleRecord* dependentModule)
{
  // TEMP
  return JsNoError;
};

/** Callback for being notified about ready module **/
static JsErrorCode
OnEnvNotifyModuleReady(JsModuleRecord referencingModule,
                       JsValueRef exceptionVar)
{
  // TEMP
  return JsNoError;
};

}
}

// ========================================================================== //
// ScriptEnvironment Implementation
// ========================================================================== //

namespace dib {
namespace script {

ScriptEnvironment::ScriptEnvironment()
{
  // Create runtime
  JsErrorCode error =
    JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &mRuntime);
  DIB_ASSERT(error == JsNoError, "Failed to create ChakraCore runtime");

  // Create context
  error = JsCreateContext(mRuntime, &mContext);
  DIB_ASSERT(error == JsNoError, "Failed to create ChakraCore Context");
  error = JsSetCurrentContext(mContext);
  DIB_ASSERT(error == JsNoError, "Failed to set current ChakraCore Context");

  // Setup
  SetupLogging();
  SetupModuleLoading();
}

// -------------------------------------------------------------------------- //

ScriptEnvironment::~ScriptEnvironment() {}

// -------------------------------------------------------------------------- //

Module*
ScriptEnvironment::LoadModule(const alflib::Path& path,
                              JsModuleRecord referencingModule)
{
  // Check if the module is already loaded
  if (mModules.count(path.GetPathString()) > 0) {
    ModuleEntry& entry = mModules[path.GetPathString()];
    entry.refCount++;
    return entry.module;
  }

  // Check if referencing module is valid otherwise set to root
  if (referencingModule == JS_INVALID_REFERENCE) {
    referencingModule = mRootModule;
  }

  // Otherwise load module
  Module* module = new Module(*this, path);
  Result result = module->Load(referencingModule);
  if (result != Result::kSuccess) {
    return nullptr;
  }

  // Add module
  ModuleEntry entry{ module, 0 };
  mModules[path.GetPathString()] = entry;

  // Return module
  return module;
}

// -------------------------------------------------------------------------- //

void
ScriptEnvironment::UnloadModule(Module* module)
{
  DIB_ASSERT(mModules.count(module->GetFile().GetPath().GetPathString()) > 0,
             "Cannot unload a module that has not been previously loaded");

  // Find entry
  ModuleEntry& entry = mModules[module->GetFile().GetPath().GetPathString()];
  entry.refCount--;
  Module* toRemove = entry.module;
  if (entry.refCount == 0) {
    mModules.erase(module->GetFile().GetPath().GetPathString());
    delete toRemove;
  }
}

// -------------------------------------------------------------------------- //

Result
ScriptEnvironment::SetupLogging()
{
  // Retrieve global object
  JsValueRef global;
  JsErrorCode error = JsGetGlobalObject(&global);
  DIB_ASSERT(error == JsNoError, "Failed to retrieve global javascript object");

  // Setup console.log
  JsValueRef dlog, logVerbose, logInfo, logWarning, logError;
  JsPropertyIdRef dlogId, logVerboseId, logInfoId, logWarningId, logErrorId;
  error = JsCreateObject(&dlog);
  DIB_ASSERT(error == JsNoError, "Failed to create global 'dlog' object");

  // Store dlog in global
  error = JsCreatePropertyId("dlog", strlen("dlog"), &dlogId);
  DIB_ASSERT(error == JsNoError, "");
  error = JsSetProperty(global, dlogId, dlog, true);
  DIB_ASSERT(error == JsNoError, "");

  // Setup 'info' function
  error = JsCreateFunction(ScriptLogVerboseFunction, nullptr, &logVerbose);
  DIB_ASSERT(error == JsNoError, "Failed to create 'verbose' function");
  error = JsCreatePropertyId("verbose", strlen("verbose"), &logVerboseId);
  DIB_ASSERT(error == JsNoError, "Failed to get 'verbose' property id");
  error = JsSetProperty(dlog, logVerboseId, logVerbose, true);
  DIB_ASSERT(error == JsNoError, "Failed to set 'dlog' property 'verbose'");

  // Setup 'info' function
  error = JsCreateFunction(ScriptLogInfoFunction, nullptr, &logInfo);
  DIB_ASSERT(error == JsNoError, "Failed to create 'info' function");
  error = JsCreatePropertyId("info", strlen("info"), &logInfoId);
  DIB_ASSERT(error == JsNoError, "Failed to get 'info' property id");
  error = JsSetProperty(dlog, logInfoId, logInfo, true);
  DIB_ASSERT(error == JsNoError, "Failed to set 'dlog' property 'info'");

  // Setup 'warning' function
  error = JsCreateFunction(ScriptLogWarningFunction, nullptr, &logWarning);
  DIB_ASSERT(error == JsNoError, "Failed to create 'warning' function");
  error = JsCreatePropertyId("warning", strlen("warning"), &logWarningId);
  DIB_ASSERT(error == JsNoError, "Failed to get 'warning' property id");
  error = JsSetProperty(dlog, logWarningId, logWarning, true);
  DIB_ASSERT(error == JsNoError, "Failed to set 'dlog' property 'warning'");

  // Setup 'error' function
  error = JsCreateFunction(ScriptLogErrorFunction, nullptr, &logError);
  DIB_ASSERT(error == JsNoError, "Failed to create 'error' function");
  error = JsCreatePropertyId("error", strlen("error"), &logErrorId);
  DIB_ASSERT(error == JsNoError, "Failed to get 'error' property id");
  error = JsSetProperty(dlog, logErrorId, logError, true);
  DIB_ASSERT(error == JsNoError, "Failed to set 'dlog' property 'error'");

  // Finalize
  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

Result
ScriptEnvironment::SetupModuleLoading()
{
  // Initialize core fake module
  JsValueRef moduleSpecifier;
  const String moduleSpecifierString(":root-module:");
  JsErrorCode error = JsCreateString(moduleSpecifierString.GetUTF8(),
                                     moduleSpecifierString.GetSize(),
                                     &moduleSpecifier);
  if (error != JsNoError) {
    return Result::kUnknownError;
  }
  error = JsInitializeModuleRecord(nullptr, moduleSpecifier, &mRootModule);
  if (error != JsNoError || mRootModule == JS_INVALID_REFERENCE) {
    return Result::kUnknownError;
  }

  // Set module callbacks
  error = JsSetModuleHostInfo(mRootModule,
                              JsModuleHostInfo_FetchImportedModuleCallback,
                              (void*)OnEnvFetchImported);
  if (error != JsNoError) {
    return Result::kUnknownError;
  }
  error =
    JsSetModuleHostInfo(mRootModule,
                        JsModuleHostInfo_FetchImportedModuleFromScriptCallback,
                        (void*)OnEnvFetchImportedFromScript);
  if (error != JsNoError) {
    return Result::kUnknownError;
  }
  error = JsSetModuleHostInfo(mRootModule,
                              JsModuleHostInfo_NotifyModuleReadyCallback,
                              (void*)OnEnvNotifyModuleReady);
  if (error != JsNoError) {
    return Result::kUnknownError;
  }

  // Finalize
  return Result::kSuccess;
}

}
}
