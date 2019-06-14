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
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
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

// Library headers
#include <ChakraCore.h>

// ========================================================================== //
// ScriptEnvironment Declaration
// ========================================================================== //

namespace dib {

/** Script environment **/
class ScriptEnvironment
{
private:
  /** Runtime **/
  JsRuntimeHandle mRuntime;
  /** Javascript context **/
  JsContextRef mContext;

private:
  /** Setup environment **/
  ScriptEnvironment();

  /** Create global functions **/
  void CreateGlobalFuncs();

  /** Create 'Mod' class **/
  void CreateModClass();

public:
  /** Returns the runtime handle **/
  JsRuntimeHandle GetRuntime() { return mRuntime; }

  /** Returns the context handle **/
  JsContextRef GetContext() { return mContext; }

  /** Check and clear the current exception **/
  void CheckAndClearException();

public:
  /** Returns the singleton script manager instance **/
  static ScriptEnvironment& Instance();

};

}