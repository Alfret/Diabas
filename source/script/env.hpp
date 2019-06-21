#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include "script/result.hpp"
#include <ChakraCore.h>
#include <alflib/file/path.hpp>
#include <queue>
#include <unordered_map>

// ========================================================================== //
// Environment Declaration
// ========================================================================== //

namespace dib {
namespace script {

/** **/
class Environment
{
public:
  /** Module **/
  struct Module
  {
    /** Owner **/
    Environment* environment;
    /** Path to module source **/
    alflib::Path path;
    /** Handle to module **/
    JsModuleRecord record;
    /** Whether the module is compiled **/
    bool isCompiled;
  };

  /** Information about a load task **/
  struct LoadTask
  {
    /** Module to load **/
    Module* module;
    /** Script source **/
    String source;
  };

  /** Information about a run task **/
  struct RunTask
  {
    /** Module to run **/
    Module* module;
  };

private:
  /** ChakraCore runtime **/
  JsRuntimeHandle mRuntime;
  /** ChakraCore context **/
  JsContextRef mContext;

  /** Root module **/
  Module* mRootModule;
  /** Map of loaded modules **/
  std::unordered_map<alflib::String, Module*> mModules;

  /** Queue of load tasks **/
  std::queue<LoadTask> mLoadTasks;
  /** Queue of run tasks  **/
  std::queue<RunTask> mRunTasks;

public:
  /** Create empty script environment **/
  Environment();

  /** Destruct script environment and unload all modules **/
  ~Environment();

  /** Run a simple script **/
  Result RunScript(const String& script,
                   JsValueRef& output,
                   void* userData = nullptr);

  /** Run a simple script from file **/
  Result RunFile(const alflib::Path& path,
                 JsValueRef& output,
                 void* userData = nullptr);

  /** Load a module **/
  Module* LoadModule(const alflib::Path& path);

  /** Instantiate a class using a constructor object **/
  JsValueRef InstantiateObject(JsValueRef constructor);

  /** Returns the prototype of the specified class in the specified module **/
  JsValueRef GetClassPrototype(Module* module, const String& className);

  /** Returns the namespace of a module **/
  JsValueRef GetModuleNamespace(Module* module);

  /** Update the environment. Loads and runs new modules **/
  void Update();

  /** Perform a debug dump. JS_INVALID_REFERENCE lists the properties of the
   * global object **/
  void ListProperties(JsValueRef object = JS_INVALID_REFERENCE,
                      const String& label = "");

private:
  /** Get a module. The module is loaded if it's the first time it's
   * referenced **/
  Module* GetModule(const alflib::Path& path, Module* parent, bool& isNew);

  /** Handle a compilation error that occurred when loading a script. The
   * function can be called with an error code. If the error code is not
   * JsErrorScriptCompile then the function returns immediately **/
  void HandleCompilationError(Module* module, JsValueRef exception);

private:
  /** Called to get notified when modules has been imported **/
  static JsErrorCode OnFetchImportedModule(
    JsModuleRecord referencingModuleRecord,
    JsValueRef specifier,
    JsModuleRecord* dependentModule);

  /** Called to get notified when modules has been imported from scripts **/
  static JsErrorCode OnFetchImportedModuleFromScript(
    JsSourceContext referencingSourceContext,
    JsValueRef specifier,
    JsModuleRecord* dependentModule);

  /** Callback to get notified when modules are ready **/
  static JsErrorCode OnNotifyModuleReady(JsModuleRecord referencingModuleRecord,
                                         JsValueRef exceptionVar);
};

}
}
