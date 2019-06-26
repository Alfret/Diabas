#include "script/util.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>
#include "core/assert.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib {
namespace script {

JsValueRef
GetGlobal()
{
  JsValueRef global;
  JsGetGlobalObject(&global);
  return global;
}

// -------------------------------------------------------------------------- //

JsValueRef
CreateUndefined()
{
  JsValueRef output;
  JsGetUndefinedValue(&output);
  return output;
}

// -------------------------------------------------------------------------- //

JsValueRef
CreateValue(bool value)
{
  JsValueRef output;
  JsBoolToBoolean(value, &output);
  return output;
}

// -------------------------------------------------------------------------- //

JsValueRef
CreateValue(s32 value)
{
  JsValueRef output;
  JsIntToNumber(value, &output);
  return output;
}

// -------------------------------------------------------------------------- //

JsValueRef
CreateValue(u32 value)
{
  JsValueRef output;
  JsIntToNumber(value, &output);
  return output;
}
// -------------------------------------------------------------------------- //

JsValueRef
CreateValue(f32 value)
{
  JsValueRef output;
  JsDoubleToNumber(static_cast<f64>(value), &output);
  return output;
}

// -------------------------------------------------------------------------- //

JsValueRef
script::CreateValue(f64 value)
{
  JsValueRef output;
  JsDoubleToNumber(value, &output);
  return output;
}

// -------------------------------------------------------------------------- //

JsValueRef
CreateString(const String& string)
{
  JsValueRef output;
  JsErrorCode error =
    JsCreateString(string.GetUTF8(), string.GetSize(), &output);
  if (error != JsNoError) {
    return JS_INVALID_REFERENCE;
  }
  return output;
}

// -------------------------------------------------------------------------- //

JsValueRef
CreateFunction(JsNativeFunction function, void* callbackState)
{
  JsValueRef output;
  JsErrorCode error = JsCreateFunction(function, callbackState, &output);
  DIB_ASSERT(error == JsNoError, "Could not create JavaScript function");
  return output;
}

// -------------------------------------------------------------------------- //

JsValueRef
CreateObject()
{
  JsValueRef object;
  JsErrorCode error = JsCreateObject(&object);
  DIB_ASSERT(error == JsNoError, "Could not create JavaScript object");
  return object;
}

// -------------------------------------------------------------------------- //

JsValueRef
CreateExternalObject(void* data, JsFinalizeCallback finalizeCallback)
{
  JsValueRef object;
  JsErrorCode error = JsCreateExternalObject(data, finalizeCallback, &object);
  DIB_ASSERT(error == JsNoError,
             "Could not create JavaScript object with external data");
  return object;
}

// -------------------------------------------------------------------------- //

bool
GetBool(JsValueRef value)
{
  bool output;
  JsErrorCode error = JsBooleanToBool(value, &output);
  DIB_ASSERT(error == JsNoError, "Failed to retrieve bool");
  return output;
}

// -------------------------------------------------------------------------- //

f32
GetF32(JsValueRef value)
{
  f64 output;
  JsNumberToDouble(value, &output);
  return static_cast<f32>(output);
}

// -------------------------------------------------------------------------- //

f64
GetF64(JsValueRef value)
{
  f64 output;
  JsNumberToDouble(value, &output);
  return output;
}
// -------------------------------------------------------------------------- //

String
GetString(JsValueRef string)
{
  size_t length = 0;
  JsCopyString(string, nullptr, 0, &length);
  auto buffer = new char8[length + 1];
  buffer[length] = 0;
  JsCopyString(string, buffer, length, &length);
  String output = buffer;
  delete[] buffer;
  return output;
}

// -------------------------------------------------------------------------- //

String
ToString(JsValueRef object)
{
  JsValueRef stringValue;
  JsConvertValueToString(object, &stringValue);
  return GetString(stringValue);
}

// -------------------------------------------------------------------------- //

bool
HasProperty(JsValueRef object, const String& property)
{
  JsPropertyIdRef id;
  JsErrorCode error =
    JsCreatePropertyId(property.GetUTF8(), property.GetSize(), &id);
  DIB_ASSERT(error == JsNoError, "Failed to create property ID");
  bool hasProperty;
  JsHasProperty(object, id, &hasProperty);
  return hasProperty;
}

// -------------------------------------------------------------------------- //

JsValueRef
GetProperty(JsValueRef object, const String& property)
{
  if (!HasProperty(object, property)) {
    return JS_INVALID_REFERENCE;
  }

  JsPropertyIdRef id;
  JsCreatePropertyId(property.GetUTF8(), property.GetSize(), &id);
  JsValueRef value;
  JsGetProperty(object, id, &value);
  return value;
}

// -------------------------------------------------------------------------- //

s32
GetPropertyInt(JsValueRef object, const String& property)
{
  JsValueRef value = GetProperty(object, property);
  int output;
  JsNumberToInt(value, &output);
  return static_cast<s32>(output);
}

// -------------------------------------------------------------------------- //

f32
GetPropertyFloat(JsValueRef object, const String& property)
{
  JsValueRef value = GetProperty(object, property);
  f64 output;
  JsNumberToDouble(value, &output);
  return static_cast<f32>(output);
}

// -------------------------------------------------------------------------- //

String
GetPropertyString(JsValueRef object, const String& property)
{
  JsValueRef value = GetProperty(object, property);
  JsValueRef stringValue;
  JsConvertValueToString(value, &stringValue);
  return GetString(stringValue);
}

// -------------------------------------------------------------------------- //

bool
SetProperty(JsValueRef object, const String& property, JsValueRef value)
{
  JsPropertyIdRef id;
  JsErrorCode error =
    JsCreatePropertyId(property.GetUTF8(), property.GetSize(), &id);
  if (error != JsNoError) {
    return false;
  }
  error = JsSetProperty(object, id, value, true);
  if (error != JsNoError) {
    return false;
  }
  return true;
}

// -------------------------------------------------------------------------- //

JsValueType
GetValueType(JsValueRef object)
{
  JsValueType type;
  JsErrorCode error = JsGetValueType(object, &type);
  DIB_ASSERT(error == JsNoError, "Failed to get type of object");
  return type;
}

// -------------------------------------------------------------------------- //

JsValueRef
GetPrototype(JsValueRef object)
{
  JsValueRef prototype;
  JsErrorCode error = JsGetPrototype(object, &prototype);
  DIB_ASSERT(error == JsNoError, "Failed to get prototype of object");
  return prototype;
}

// -------------------------------------------------------------------------- //

JsErrorCode
CallMethod(JsValueRef method,
           std::vector<JsValueRef> arguments,
           JsValueRef& output)
{
  return JsCallFunction(
    method, arguments.data(), static_cast<u16>(arguments.size()), &output);
}

// -------------------------------------------------------------------------- //

bool
HandleException(JsErrorCode errorCode)
{
  // Return if the error is not a script exception
  if (errorCode != JsErrorScriptException) {
    return false;
  }

  // Check if there is an exception
  bool hasException = false;
  JsHasException(&hasException);
  if (!hasException) {
    return false;
  }

  // Retrieve exception
  JsValueRef exception;
  JsErrorCode error = JsGetAndClearException(&exception);
  DIB_ASSERT(error == JsNoError, "Failed to retrieve exception");

  // Retrieve message
  JsPropertyIdRef messageId;
  JsCreatePropertyId("message", strlen("message"), &messageId);
  DIB_ASSERT(error == JsNoError, "Failed to retrieve exception message");
  JsValueRef message;
  JsGetProperty(exception, messageId, &message);
  DIB_ASSERT(error == JsNoError, "Failed to retrieve exception message");

  // Log message
  DLOG_ERROR("Exception occured when running script: {}", GetString(message));
  return true;
}

// -------------------------------------------------------------------------- //

void
ListProperties(JsValueRef object, const String& label)
{
  // Retrieve global object
  JsValueRef global;
  JsGetGlobalObject(&global);
  if (object == JS_INVALID_REFERENCE) {
    object = global;
  }

  // Retrieve the property list
  JsValueRef propertyNames;
  JsGetOwnPropertyNames(object, &propertyNames);

  // Retrieve length
  s32 length = GetPropertyInt(propertyNames, "length");
  DLOG_VERBOSE("Listing properties{}",
               label.GetLength() > 0 ? String(" [") + label + "]" : "");
  for (s32 i = 0; i < length; i++) {
    String indexString = String::ToString(i);
    JsValueRef element = GetProperty(propertyNames, indexString);
    String stringValue = ToString(element);
    DLOG_VERBOSE("  [{}]: {}", i, stringValue);
  }
}

}
}