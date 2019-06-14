#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "alflib/file/file.hpp"
#include "core/types.hpp"
#include <ChakraCore.h>

// ========================================================================== //
// Script Declaration
// ========================================================================== //

namespace dib {

/** Script **/
class Script
{
public:
  /** Script-related errors **/
  enum class Error
  {
    /** No error **/
    kNoError,
    /** Unknown error **/
    kUnknownError,
    /** Script file could not be found **/
    kScriptNotFound,
    /** Main class could not be found **/
    kMainClassNotFound,
    /** Compilation error **/
    kCompileError
  };

protected:
  /** File **/
  alflib::File mFile;
  /** Source **/
  String mSource;
  /** Script **/
  JsValueRef mScript;
  /** Serialized script **/
  JsValueRef mScriptBuffer;

public:
  /** Load a script from file **/
  explicit Script(const alflib::File& file);

  /** Serialize/Compile the script **/
  Error Compile();

  /** Returns whether script is compiled **/
  bool IsCompiled() const { return mScriptBuffer != JS_INVALID_REFERENCE; }

};

}

// ========================================================================== //
// ModScript Declaration
// ========================================================================== //

namespace dib {

/** Script that represents the main 'Mod' script **/
class ModScript : Script
{
private:
  /** 'Mod' class name **/
  String mClassName;
  /** Mod object instance **/
  JsValueRef mInstance;

  /** 'init' function **/
  JsValueRef mInitFunction;
  /** 'update' function **/
  JsValueRef mUpdateFunction;

public:
  /** Load mod script from path. The name of the main 'Mod' class must be
   * specified so that it can be instantiated **/
  ModScript(const alflib::File& file, const String& className);

  /** Load the script **/
  Error Load();

  /** Call the 'init' function in the mod **/
  void Init() const;

  /** Call the 'update' function in the mod **/
  void Update(f32 delta) const;

private:
  /** Load a function from the instance **/
  JsErrorCode LoadFunction(const String& name, JsValueRef* function) const;
};

}