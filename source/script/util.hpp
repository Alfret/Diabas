#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include <ChakraCore.h>
#include <vector>

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib {
namespace script {

/** Returns the global object **/
JsValueRef
GetGlobal();

// -------------------------------------------------------------------------- //

/** Create undefined value **/
JsValueRef
CreateUndefined();

// -------------------------------------------------------------------------- //

/** Create a JavaScript number from a bool **/
JsValueRef
CreateValue(bool value);

// -------------------------------------------------------------------------- //

/** Create a JavaScript number from an s32 **/
JsValueRef
CreateValue(s32 value);

// -------------------------------------------------------------------------- //

/** Create a JavaScript number from an u32 **/
JsValueRef
CreateValue(u32 value);

// -------------------------------------------------------------------------- //

/** Create a JavaScript number from an f32 **/
JsValueRef
CreateValue(f32 value);

// -------------------------------------------------------------------------- //

/** Create a JavaScript number from an f64 **/
JsValueRef
CreateValue(f64 value);

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

/** Retrieve bool value **/
bool
GetBool(JsValueRef value);

// -------------------------------------------------------------------------- //

/** Retrieve s32 value **/
s32
GetS32(JsValueRef value);

// -------------------------------------------------------------------------- //

/** Retrieve u32 value **/
u32
GetU32(JsValueRef value);

// -------------------------------------------------------------------------- //

/** Retrieve f32 value **/
f32
GetF32(JsValueRef value);

// -------------------------------------------------------------------------- //

/** Retrieve f64 value **/
f64
GetF64(JsValueRef value);

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

// -------------------------------------------------------------------------- //

/** Returns the type of a JavaScript object **/
JsValueType
GetValueType(JsValueRef object);

// -------------------------------------------------------------------------- //

/** Returns the prototype of an object **/
JsValueRef
GetPrototype(JsValueRef object);

// -------------------------------------------------------------------------- //

/** Call a function with the specified arguments. The output is stored in the
 * 'output' parameter **/
JsErrorCode
CallMethod(JsValueRef method,
           std::vector<JsValueRef> arguments,
           JsValueRef& output);

// -------------------------------------------------------------------------- //

/** Gets the last exception, clears the exception state and then finally returns
 * the exception message**/
String
GetAndClearException();

// -------------------------------------------------------------------------- //

/** Handle an exception that occurred in a script. The function takes the error
 * code that last happened, and if it's the code 'JsErrorScriptException' then
 * the exception is printed and the function returns true. If the error is
 * something or not an error at all then false is returned and nothing is done
 * **/
bool
HandleException(JsErrorCode errorCode);

// -------------------------------------------------------------------------- //

/** Perform a debug dump. JS_INVALID_REFERENCE lists the properties of the
 * global object **/
void
ListProperties(JsValueRef object = JS_INVALID_REFERENCE,
               const String& label = "");

}
}
