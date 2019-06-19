#include "script/util.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

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
CreateValue(f32 value)
{
  JsValueRef output;
  JsDoubleToNumber(static_cast<f64>(value), &output);
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

f32
GetF32(JsValueRef value)
{
  f64 output;
  JsNumberToDouble(value, &output);
  return static_cast<f32>(output);
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

}
}