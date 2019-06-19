#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include "script/env.hpp"

// ========================================================================== //
// Script Declaration
// ========================================================================== //

namespace dib::script {

/** **/
class Script
{
private:
  /** Environment **/
  Environment& mEnvironment;
  /** Script module **/
  Environment::Module* mModule = nullptr;

  /** Path to script module **/
  alflib::Path mPath;
  /** Main class name **/
  String mClassName;

  /** Class instance **/
  JsValueRef mInstance = JS_INVALID_REFERENCE;
  /** Init function **/
  JsValueRef mInitFunction = JS_INVALID_REFERENCE;
  /** Update function **/
  JsValueRef mUpdateFunction = JS_INVALID_REFERENCE;

public:
  /** Create script. This does not yet load the script **/
  Script(Environment& environment,
         const alflib::Path& path,
         const String& className = "");

  ~Script();

  Result Load();

  void Init();

  void Update(f32 delta);
};

}