#include "script/script.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "script/util.hpp"

// ========================================================================== //
// Script Implementation
// ========================================================================== //

namespace dib::script {

Script::Script(Environment& environment)
  : mEnvironment(environment)
{}

// -------------------------------------------------------------------------- //

Script::~Script()
{
  JsRelease(mUpdateFunction, nullptr);
  JsRelease(mInitFunction, nullptr);
  JsRelease(mInstance, nullptr);
}

// -------------------------------------------------------------------------- //

Result
Script::Load(const Path& path, const String& className)
{
  mPath = path;
  mClassName = className;

  // Load the module
  mModule = mEnvironment.LoadModule(mPath);
  if (!mModule) {
    return Result::kModuleNotFound;
  }

  // Find mod class
  JsValueRef moduleNamespace = mEnvironment.GetModuleNamespace(mModule);
  JsValueRef cls = JS_INVALID_REFERENCE;
  if (HasProperty(moduleNamespace, "default")) {
    cls = GetProperty(moduleNamespace, "default");
  } else {
    cls = GetProperty(moduleNamespace, mClassName);
    if (cls == JS_INVALID_REFERENCE) {
      return Result::kClassNotFound;
    }
  }

  // Instantiate mod
  mInstance = mEnvironment.InstantiateObject(cls);
  if (mInstance == JS_INVALID_REFERENCE) {
    return Result::kClassNotFound;
  }
  JsAddRef(mInstance, nullptr);

  // Retrieve functions
  mInitFunction = GetProperty(mInstance, "init");
  JsAddRef(mInitFunction, nullptr);
  mUpdateFunction = GetProperty(mInstance, "update");
  JsAddRef(mUpdateFunction, nullptr);

  // Success
  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

void
Script::Init()
{
  JsValueRef args[] = { mInstance };
  JsValueRef output;
  JsCallFunction(mInitFunction, args, 1, &output);
  (void)output;
}

// -------------------------------------------------------------------------- //

void
Script::Update(f32 delta)
{
  JsValueRef args[] = { mInstance, CreateValue(delta) };
  JsValueRef output;
  JsCallFunction(mUpdateFunction, args, 2, &output);
  (void)output;
}

}