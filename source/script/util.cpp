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