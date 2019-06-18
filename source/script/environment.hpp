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

#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <unordered_map>
#include <ChakraCore.h>
#include "core/macros.hpp"
#include "core/types.hpp"
#include "script/result.hpp"
#include <alflib/file/path.hpp>

// ========================================================================== //
// ScriptEnvironment Declaration
// ========================================================================== //

namespace dib {
namespace script {

DIB_FORWARD_DECLARE_CLASS(Module);

/** Script environment **/
class ScriptEnvironment
{
  friend Module;

public:
  /** Entry in modules list **/
  struct ModuleEntry
  {
    /** Module handle **/
    Module* module;
    /** Number of live usages of this module **/
    u32 refCount;
  };

private:
  /** ChakraCore runtime **/
  JsRuntimeHandle mRuntime;
  /** ChakraCore context **/
  JsContextRef mContext;

  /** Root module **/
  JsModuleRecord mRootModule;

  /** Map of loaded modules to their paths (relative to game root) **/
  std::unordered_map<String, ModuleEntry> mModules;

public:
  /** Create script environment **/
  ScriptEnvironment();

  /** Destruct script environment **/
  ~ScriptEnvironment();

  /** Load a module **/
  Module* LoadModule(const alflib::Path& path,
                     JsModuleRecord referencingModule = JS_INVALID_REFERENCE);

  /** Returns the root module **/
  JsModuleRecord GetRootModule() const { return mRootModule; }

private:
  /** Unload module. Called by the module itself **/
  void UnloadModule(Module* module);

  /** Setup logging functions **/
  Result SetupLogging();

  /** Setup module loading **/
  Result SetupModuleLoading();

};

}
}