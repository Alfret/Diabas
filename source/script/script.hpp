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
  Path mPath = Path{ "" };
  /** Main class name **/
  String mClassName = "";

  /** Class instance **/
  JsValueRef mInstance = JS_INVALID_REFERENCE;
  /** Init function **/
  JsValueRef mInitFunction = JS_INVALID_REFERENCE;
  /** Update function **/
  JsValueRef mUpdateFunction = JS_INVALID_REFERENCE;

public:
  /** Create script. This does not yet load the script **/
  Script(Environment& environment);

  /** Destruct, destroying the instance **/
  ~Script();

  /** Load the script from the specified path and with an optional class name to
   * use when looking for the exported mod class **/
  Result Load(const Path& path, const String& className = "");

  /** Initialize script **/
  void Init();

  /** Update script **/
  void Update(f32 delta);

  /** Returns the instance of the mod class in the script **/
  JsValueRef GetInstance() const { return mInstance; }
};

}