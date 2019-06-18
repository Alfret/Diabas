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

#include "script/module.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "alflib/file/file_io.hpp"
#include "script/environment.hpp"

// ========================================================================== //
// Module Implementation
// ========================================================================== //

namespace dib {
namespace script {

Module::Module(ScriptEnvironment& environment, const alflib::File& file)
  : mEnvironment(environment)
  , mFile(file)
  , mSource("")
  , mModule(JS_INVALID_REFERENCE)
{}

// -------------------------------------------------------------------------- //

Module::~Module()
{
  mEnvironment.UnloadModule(this);
}

// -------------------------------------------------------------------------- //

Result
Module::Load(JsModuleRecord referencingModule)
{
  // Setup specifier
  JsValueRef specifier;
  JsErrorCode error = JsCreateString(mFile.GetPath().GetPathString().GetUTF8(),
                                     mFile.GetPath().GetPathString().GetSize(),
                                     &specifier);
  if (error != JsNoError) {
    return Result::kUnknownError;
  }

  // Create module
  error = JsInitializeModuleRecord(referencingModule, specifier, &mModule);
  if (error != JsNoError || mModule == JS_INVALID_REFERENCE) {
    return Result::kUnknownError;
  }

  // Store the module pointer in module
  error = JsSetModuleHostInfo(mModule, JsModuleHostInfo_HostDefined, this);
  if (error != JsNoError) {
    return Result::kUnknownError;
  }

  // Load the source
  alflib::FileIO io(mFile);
  const alflib::FileResult result = io.Open(
    alflib::FileIO::Flag::kRead | alflib::FileIO::Flag::kShareReadWrite);
  if (result == alflib::FileResult::kSuccess) {
    io.Read(mSource);
  }

  // Parse module source
  JsValueRef exceptionValue = JS_INVALID_REFERENCE;
  error = JsParseModuleSource(mModule,
                              reinterpret_cast<JsSourceContext>(this),
                              (BYTE*)mSource.GetUTF8(),
                              mSource.GetSize(),
                              JsParseModuleSourceFlags_DataIsUTF8,
                              &exceptionValue);
  if (error != JsNoError) {
    return Result::kUnknownError;
  }

  // Finalize
  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

Result
Module::Run()
{
  // Run module
  JsValueRef evaluationResult = JS_INVALID_REFERENCE;
  JsErrorCode error = JsModuleEvaluation(mModule, &evaluationResult);
  if (error != JsNoError) {
    return Result::kUnknownError;
  }
  return Result::kSuccess;
}

}
}