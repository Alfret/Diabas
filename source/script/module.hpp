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

#include "core/macros.hpp"
#include "core/types.hpp"
#include "script/result.hpp"
#include <ChakraCore.h>
#include <alflib/file/file.hpp>

// ========================================================================== //
// Module Declaration
// ========================================================================== //

namespace dib {
namespace script {

DIB_FORWARD_DECLARE_CLASS(ScriptEnvironment);

/** Script module **/
class Module
{
private:
  /** Environment that modules belongs in **/
  ScriptEnvironment& mEnvironment;

  /** File of the module **/
  alflib::File mFile;
  /** Source code **/
  String mSource;

  /** Module handle **/
  JsModuleRecord mModule;

public:
  /** Create module. This does not not load the module **/
  explicit Module(ScriptEnvironment& environment, const alflib::File& file);

  /** Deleted copy-constructor **/
  Module(const Module&) = delete;

  /** Destroy module **/
  ~Module();

  /** Deleted copy-assignment **/
  Module& operator=(const Module&) = delete;

  /** Load the actual module **/
  Result Load(JsModuleRecord referencingModule = JS_INVALID_REFERENCE);

  /** Run the module to load all symbols **/
  Result Run();

  /** Returns the environment that this script belongs to **/
  ScriptEnvironment& GetEnvironment() { return mEnvironment; }

  /** Returns the file of the script module **/
  alflib::File GetFile() const { return mFile; }

  /** Returns the module handle **/
  JsModuleRecord GetHandle() const { return mModule; }
};

}
}