#include "script/script.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

// Library headers
#include <ChakraCommon.h>
#include <ChakraCore.h>
#include <alflib/file/file_io.hpp>

// Project headers
#include "core/assert.hpp"
#include "environment.hpp"

// ========================================================================== //
// Script Implementation
// ========================================================================== //

namespace dib {

Script::Script(const alflib::File& file)
  : mFile(file)
  , mSource("")
  , mScript(JS_INVALID_REFERENCE)
  , mScriptBuffer(JS_INVALID_REFERENCE)
{}

// -------------------------------------------------------------------------- //

Script::Error
Script::Compile()
{
  using namespace alflib;
  ScriptEnvironment::Instance();

  // Load the source
  FileIO io(mFile);
  const FileResult result =
    io.Open(FileIO::Flag::kRead | FileIO::Flag::kShareReadWrite);
  if (result == FileResult::kSuccess) {
    io.Read(mSource);
  }

  // Load script source into object
  JsValueRef sourceArray, sourceBuffer;
  JsErrorCode error = JsCreateExternalArrayBuffer(
    mSource.GetData(), mSource.GetSize(), nullptr, nullptr, &sourceArray);
  if (error != JsNoError) {
    return Error::kUnknownError;
  }

  // Serialize
  error = JsSerialize(sourceArray, &sourceBuffer, JsParseScriptAttributeNone);
  if (error != JsNoError) {
    return Error::kCompileError;
  }

  // Setup load callback
  static auto LoadCallback = [](JsSourceContext context,
                         JsValueRef* scriptBuffer,
                         JsParseScriptAttributes* attributes) {
    return true;
  };

  // Setup script URL
  JsValueRef url;
  error = JsCreateString(mFile.GetPath().GetName().GetUTF8(),
                 mFile.GetPath().GetName().GetSize(),
                 &url);
  if (error != JsNoError) {
    return Error::kUnknownError;
  }

  // Run script once to load symbols
  JsValueRef output;
  error = JsRunSerialized(sourceBuffer,
                  LoadCallback,
                  reinterpret_cast<JsSourceContext>(this),
                  url,
                  &output);
  if (error != JsNoError) {
    if (error == JsErrorScriptException) {
      ScriptEnvironment::Instance().CheckAndClearException();
    }
    return Error::kUnknownError;
  }

  // Finalize
  return Error::kNoError;
}

}

// ========================================================================== //
// ModScript Implementation
// ========================================================================== //

namespace dib {
   
//void
//EnumPropsObj(JsValueRef obj, const String& name)
//{
//  JsValueRef glob;
//  JsErrorCode error = JsGetGlobalObject(&glob);
//  DIB_ASSERT(error == JsNoError, "");
//  
//  JsPropertyIdRef maId;
//  error = JsCreatePropertyId(name.GetUTF8(), name.GetSize(), &maId);
//  DIB_ASSERT(error == JsNoError, "");
//  error = JsSetProperty(glob, maId, obj, true);
//  DIB_ASSERT(error == JsNoError, "");
//
//  const String script = "console.log(\"Enumerating objects:\");"
//                        "console.log(new CoreMod().init())";
//
//  JsValueRef str, nam;
//  error = JsCreateString(script.GetUTF8(), script.GetSize(), &str);
//  DIB_ASSERT(error == JsNoError, "");
//  error = JsCreateString("debug_script", strlen("debug_script"), &nam);
//  DIB_ASSERT(error == JsNoError, "");
//  JsValueRef rrrrr;
//  error = JsRun(str, 0, nam, JsParseScriptAttributeNone, &rrrrr);
//  if (error == JsErrorScriptException) {
//    ScriptEnvironment::Instance().CheckAndClearException();
//  }
//  DIB_ASSERT(error == JsNoError, "");
//}

ModScript::ModScript(const alflib::File& file, const String& className)
  : Script(file)
  , mClassName(className)
  , mInstance(JS_INVALID_REFERENCE)
  , mInitFunction(JS_INVALID_REFERENCE)
  , mUpdateFunction(JS_INVALID_REFERENCE)
{}

// -------------------------------------------------------------------------- //

Script::Error
ModScript::Load()
{
  // Compile if neccessary
  if (!IsCompiled()) {
    Compile();
  }

  // Instantiate
  const String code = "new " + mClassName + "();";
  JsValueRef script, scriptEnv;
  JsCreateString(code.GetUTF8(), code.GetSize(), &script);
  JsCreateString(":env:", strlen(":env:"), &scriptEnv);
  const JsErrorCode error =
    JsRun(script, 0, scriptEnv, JsParseScriptAttributeNone, &mInstance);
  DIB_ASSERT(error == JsNoError, "Failed to instantiate class");

  // Retrieve functions
  LoadFunction("init", &mInitFunction);
  LoadFunction("update", &mUpdateFunction);

  return Error::kNoError;
}

// -------------------------------------------------------------------------- //

void
ModScript::Init() const
{
  DIB_ASSERT(mInitFunction != JS_INVALID_REFERENCE,
             "Load must be called for ModScript before running any functions");
  JsValueRef result;
  JsValueRef args[] = { mInstance };
  JsCallFunction(mInitFunction, args, 1, &result);
}

// -------------------------------------------------------------------------- //

void
ModScript::Update(f32 delta) const
{
  DIB_ASSERT(mUpdateFunction != JS_INVALID_REFERENCE,
             "Load must be called for ModScript before running any functions");
  JsValueRef deltaValue, result;
  JsDoubleToNumber(delta, &deltaValue); 
  JsValueRef args[] = { mInstance, deltaValue };
  JsCallFunction(mUpdateFunction, args, 2, &result);
}

// -------------------------------------------------------------------------- //

JsErrorCode
ModScript::LoadFunction(const String& name, JsValueRef* function) const
{
  // Retrieve prototype
  JsPropertyIdRef prototypeId;
  JsErrorCode error =
    JsCreatePropertyId("__proto__", strlen("__proto__"), &prototypeId);
  if (error != JsNoError) {
    return error;
  }
  JsValueRef prototype;
  error = JsGetProperty(mInstance, prototypeId, &prototype);
  if (error != JsNoError) {
    return error;
  }

  // Retrieve function
  JsPropertyIdRef functionId;
  error = JsCreatePropertyId(name.GetUTF8(), name.GetSize(), &functionId);
  if (error != JsNoError) {
    return error;
  }
  return JsGetProperty(prototype, functionId, function);
}

}
