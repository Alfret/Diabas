#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <ChakraCore.h>
#include "core/string.hpp"

// ========================================================================== //
// Script Declaration
// ========================================================================== //

namespace dib {

/** Script **/
class Script
{
private:


public:
  explicit Script(const String& path);

  void Run(const String& function);

};

}

// ========================================================================== //
// ScriptManager Declaration
// ========================================================================== //

namespace dib {

/** Script manager **/
class ScriptManager
{
private:
  /** Runtime **/
  JsRuntimeHandle mRuntime;
  /** Javascript context **/
  JsContextRef mContext;

private:
  ScriptManager();

public:
  /** Returns the runtime handle **/
  JsRuntimeHandle GetRuntime() const { return mRuntime; }

  /** Returns the context handle **/
  JsContextRef GetContext() const { return mContext; }

private:
  /** Returns the singleton script manager instance **/
  static ScriptManager& Instance();

};

}