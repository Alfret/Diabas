#include "script/script.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <ChakraCore.h>
#include "core/assert.hpp"

// ========================================================================== //
// Script Implementation
// ========================================================================== //

namespace dib {

Script::Script(const String& path)
{
  // Load the source

  ScriptManager::GetContext();

  JsValueRef res;
  JsRunScript(L"(()=>{return \'Hello world!\';})()", 0, L"", &res);
  JsValueRef resStr;
  JsConvertValueToString(res, &resStr);
  const wchar_t* outStr;
  size_t outLen;
  JsStringToPointer(resStr, &outStr, &outLen);

  int y = 0;

}

// -------------------------------------------------------------------------- //

void
Script::Run(const String& function)
{
  
}

}

// ========================================================================== //
// ScriptManager Implementation
// ========================================================================== //

namespace dib {

ScriptManager::ScriptManager()
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
}

// -------------------------------------------------------------------------- //

ScriptManager&
ScriptManager::Instance()
{
  static ScriptManager instance;
  return instance;
}

}
