#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include <ChakraCore.h>

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib {
namespace script {

/** Returns the global object **/
JsValueRef
GetGlobal();

// -------------------------------------------------------------------------- //

/** Create a JavaScript number from an f32 **/
JsValueRef
CreateValue(f32 value);

// -------------------------------------------------------------------------- //

/** Create a JavaScript string **/
JsValueRef
CreateString(const String& string);

// -------------------------------------------------------------------------- //

/** Create a function with an optional user data (callbackState) that is passed
 * to the native function **/
JsValueRef
CreateFunction(JsNativeFunction function, void* callbackState = nullptr);

// -------------------------------------------------------------------------- //

/** Creates an empty object **/
JsValueRef
CreateObject();

// -------------------------------------------------------------------------- //

JsValueRef
CreateExternalObject(void* data, JsFinalizeCallback finalizeCallback = nullptr);

// -------------------------------------------------------------------------- //

/** Retrieve f32 value **/
f32
GetF32(JsValueRef value);

// -------------------------------------------------------------------------- //

/** Retrieve string **/
String
GetString(JsValueRef string);

// -------------------------------------------------------------------------- //

template<typename T>
T*
GetExternalData(JsValueRef object)
{
  void* data;
  JsGetExternalData(object, &data);
  return static_cast<T*>(data);
}

// -------------------------------------------------------------------------- //

/** Converts an object to a string **/
String
ToString(JsValueRef object);

// -------------------------------------------------------------------------- //

/** Returns whether an object has the specified property **/
bool
HasProperty(JsValueRef object, const String& property);

// -------------------------------------------------------------------------- //

/** Returns the property of an object. If the property does not exist then
 * JS_INVALID_REFERENCE is returned instead **/
JsValueRef
GetProperty(JsValueRef object, const String& property);

// -------------------------------------------------------------------------- //

/** Returns a property of an object as an integer **/
s32
GetPropertyInt(JsValueRef object, const String& property);

// -------------------------------------------------------------------------- //

/** Returns a property of an object as an f32 **/
f32
GetPropertyFloat(JsValueRef object, const String& property);

// -------------------------------------------------------------------------- //

/** Returns a property of an object as a string **/
String
GetPropertyString(JsValueRef object, const String& property);

// -------------------------------------------------------------------------- //

/** Set the property of an object to the specified value. If the property could
 * not be set then false is returned **/
bool
SetProperty(JsValueRef object, const String& property, JsValueRef value);

}
}