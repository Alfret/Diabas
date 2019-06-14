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

// Library headers
#include <dlog.hpp>

// Project headers
#include "core/assert.hpp"
#include "core/types.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib {

/** Logs a verbose message **/
JsValueRef CALLBACK
ScriptLogVerboseFunction(JsValueRef callee,
                      bool isConstructCall,
                      JsValueRef* arguments,
                      u16 argumentCount,
                      void* callbackState)
{
  JsValueRef messageValue;
  const JsErrorCode error = JsConvertValueToString(arguments[1], &messageValue);
  if (error != JsNoError) {
    return JS_INVALID_REFERENCE;
  }
  const char16* message;
  size_t messageLength;
  JsStringToPointer(messageValue, &message, &messageLength);
  DLOG_VERBOSE(String(message).GetUTF8());
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

/** Logs an info message **/
JsValueRef CALLBACK
ScriptLogInfoFunction(JsValueRef callee,
                  bool isConstructCall,
                  JsValueRef* arguments,
                  u16 argumentCount,
                  void* callbackState)
{
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
JsValueRef CALLBACK
ScriptLogWarningFunction(JsValueRef callee,
                      bool isConstructCall,
                      JsValueRef* arguments,
                      u16 argumentCount,
                      void* callbackState)
{
  JsValueRef messageValue;
  const JsErrorCode error = JsConvertValueToString(arguments[1], &messageValue);
  if (error != JsNoError) {
    return JS_INVALID_REFERENCE;
  }
  const char16* message;
  size_t messageLength;
  JsStringToPointer(messageValue, &message, &messageLength);
  DLOG_WARNING(String(message).GetUTF8());
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

/** Logs an info message **/
JsValueRef CALLBACK
ScriptLogErrorFunction(JsValueRef callee,
                      bool isConstructCall,
                      JsValueRef* arguments,
                      u16 argumentCount,
                      void* callbackState)
{
  JsValueRef messageValue;
  const JsErrorCode error = JsConvertValueToString(arguments[1], &messageValue);
  if (error != JsNoError) {
    return JS_INVALID_REFERENCE;
  }
  const char16* message;
  size_t messageLength;
  JsStringToPointer(messageValue, &message, &messageLength);
  DLOG_ERROR(String(message).GetUTF8());
  return JS_INVALID_REFERENCE;
}




}

// ========================================================================== //
// ScriptEnvironment Implementation
// ========================================================================== //

namespace dib {

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

  // Setup classes
  CreateGlobalFuncs();
  CreateModClass();
}

// -------------------------------------------------------------------------- //

void
ScriptEnvironment::CreateGlobalFuncs()
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

// -------------------------------------------------------------------------- //

void
ScriptEnvironment::CreateModClass()
{
  // Retrieve global object
  JsValueRef global;
  JsErrorCode error = JsGetGlobalObject(&global);
  DIB_ASSERT(error == JsNoError, "Failed to retrieve global javascript object");

  // Source for 'Mod' class
  static char8 source[] = "class Mod\n"
                          "{\n"
                          "  init() {}\n"
                          "  update(delta) {}\n"
                          "}";

  // Execute source to load class
  JsValueRef sourceArray, envName;
  error = JsCreateExternalArrayBuffer(
    source, strlen(source), nullptr, nullptr, &sourceArray);
  JsCreateString(":env:", strlen(":env:"), &envName);

  JsValueRef result;
  JsRun(sourceArray, 0, envName, JsParseScriptAttributeNone, &result);
  DIB_ASSERT(error == JsNoError, "Could not load 'Mod' class");
}

// -------------------------------------------------------------------------- //

void
ScriptEnvironment::CheckAndClearException()
{
  Instance();

  // Get and clear exception
  JsValueRef exception;
  JsGetAndClearException(&exception);

  // Retrieve message property
  JsValueRef message;
  JsPropertyIdRef id;
  JsErrorCode error = JsCreatePropertyId("message", strlen("message"), &id);
  DIB_ASSERT(error == JsNoError,
             "Failed to get property id: exception::message");
  error = JsGetProperty(exception, id, &message);
  DIB_ASSERT(error == JsNoError, "Failed to get property: exception::message");

  // Retrieve message
  size_t messageLength;
  error = JsCopyString(message, nullptr, 0, &messageLength);
  DIB_ASSERT(error == JsNoError, "");
  char8* messageBuffer = new char8[messageLength];
  error = JsCopyString(message, messageBuffer, messageLength, &messageLength);
  DIB_ASSERT(error == JsNoError, "");

  // Log message
  DLOG_ERROR("Script exception: {}", String(messageBuffer));
}

// -------------------------------------------------------------------------- //

ScriptEnvironment&
ScriptEnvironment::Instance()
{
  static ScriptEnvironment instance;
  return instance;
}

}