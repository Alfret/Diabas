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

#include "script/expose/expose_base.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/assert.hpp"
#include "core/types.hpp"
#include "script/util.hpp"
#include <ChakraCore.h>
#include <dlog.hpp>

// ========================================================================== //
// Private Functions
// ========================================================================== //

namespace dib::script {

/** Logs a verbose message **/
JsValueRef CHAKRA_CALLBACK
ScriptLogVerboseFunction([[maybe_unused]] JsValueRef callee,
                         bool isConstructCall,
                         JsValueRef* arguments,
                         u16 argumentCount,
                         [[maybe_unused]] void* callbackState)
{
  DIB_ASSERT(!isConstructCall, "Function is not a destructor call");

  // Output string
  String output = "";

  // Combine all arguments
  for (u16 i = 1; i < argumentCount; i++) {
    output += ToString(arguments[i]);
  }

  // Log
  DLOG_VERBOSE(output.GetUTF8());
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

/** Logs an info message **/
JsValueRef CHAKRA_CALLBACK
ScriptLogInfoFunction([[maybe_unused]] JsValueRef callee,
                      bool isConstructCall,
                      JsValueRef* arguments,
                      u16 argumentCount,
                      [[maybe_unused]] void* callbackState)
{
  DIB_ASSERT(!isConstructCall, "Function is not a destructor call");

  // Output string
  String output = "";

  // Combine all arguments
  for (u16 i = 1; i < argumentCount; i++) {
    output += ToString(arguments[i]);
  }

  // Log
  DLOG_INFO(output.GetUTF8());
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

/** Logs an info message **/
JsValueRef CHAKRA_CALLBACK
ScriptLogWarningFunction([[maybe_unused]] JsValueRef callee,
                         bool isConstructCall,
                         JsValueRef* arguments,
                         u16 argumentCount,
                         [[maybe_unused]] void* callbackState)
{
  DIB_ASSERT(!isConstructCall, "Function is not a destructor call");

  // Output string
  String output = "";

  // Combine all arguments
  for (u16 i = 1; i < argumentCount; i++) {
    output += ToString(arguments[i]);
  }

  // Log
  DLOG_WARNING(output.GetUTF8());
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

/** Logs an info message **/
JsValueRef CHAKRA_CALLBACK
ScriptLogErrorFunction([[maybe_unused]] JsValueRef callee,
                       bool isConstructCall,
                       JsValueRef* arguments,
                       u16 argumentCount,
                       [[maybe_unused]] void* callbackState)
{
  DIB_ASSERT(!isConstructCall, "Function is not a destructor call");

  // Output string
  String output = "";

  // Combine all arguments
  for (u16 i = 1; i < argumentCount; i++) {
    output += ToString(arguments[i]);
  }

  // Log
  DLOG_ERROR(output.GetUTF8());
  return JS_INVALID_REFERENCE;
}

}

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::script {

void
ExposeLogging()
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
}

}