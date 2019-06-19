#include "script/expose/expose_graphics.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/assert.hpp"
#include "script/util.hpp"
#include "graphics/color.hpp"
#include <dlog.hpp>

// ========================================================================== //
// Color Functions
// ========================================================================== //

namespace dib::script {

static JsValueRef COLOR_PROTOTYPE = JS_INVALID_REFERENCE;

// -------------------------------------------------------------------------- //

JsValueRef CHAKRA_CALLBACK
ScriptColorGetRed(JsValueRef callee,
                  bool isConstruct,
                  [[maybe_unused]] JsValueRef* arguments,
                  u16 argumentCount,
                  [[maybe_unused]] void* callbackState)
{
  DIB_ASSERT(!isConstruct, "Color constructor must be a construct call");
  DIB_ASSERT(argumentCount == 1, "ScriptColorGetRed expects no arguments");
  graphics::Color* color = GetExternalData<graphics::Color>(arguments[0]);
  DIB_ASSERT(color != nullptr, "External object data is null");
  return CreateValue(color->Red());
}

// -------------------------------------------------------------------------- //

JsValueRef CHAKRA_CALLBACK
ScriptColorGetGreen(JsValueRef callee,
                    bool isConstruct,
                    [[maybe_unused]] JsValueRef* arguments,
                    u16 argumentCount,
                    [[maybe_unused]] void* callbackState)
{
  DIB_ASSERT(!isConstruct, "Color constructor must be a construct call");
  DIB_ASSERT(argumentCount == 1, "ScriptColorGetGreen expects no arguments");
  graphics::Color* color = GetExternalData<graphics::Color>(arguments[0]);
  DIB_ASSERT(color != nullptr, "External object data is null");
  return CreateValue(color->Green());
}

// -------------------------------------------------------------------------- //

JsValueRef CHAKRA_CALLBACK
ScriptColorGetBlue(JsValueRef callee,
                   bool isConstruct,
                   [[maybe_unused]] JsValueRef* arguments,
                   u16 argumentCount,
                   [[maybe_unused]] void* callbackState)
{
  DIB_ASSERT(!isConstruct, "Color constructor must be a construct call");
  DIB_ASSERT(argumentCount == 1, "ScriptColorGetBlue expects no arguments");
  graphics::Color* color = GetExternalData<graphics::Color>(arguments[0]);
  DIB_ASSERT(color != nullptr, "External object data is null");
  return CreateValue(color->Blue());
}

// -------------------------------------------------------------------------- //

JsValueRef CHAKRA_CALLBACK
ScriptColorGetAlpha(JsValueRef callee,
                    bool isConstruct,
                    [[maybe_unused]] JsValueRef* arguments,
                    u16 argumentCount,
                    [[maybe_unused]] void* callbackState)
{
  DIB_ASSERT(!isConstruct, "Color constructor must be a construct call");
  DIB_ASSERT(argumentCount == 1, "ScriptColorGetAlpha expects no arguments");
  graphics::Color* color = GetExternalData<graphics::Color>(arguments[0]);
  DIB_ASSERT(color != nullptr, "External object data is null");
  return CreateValue(color->Alpha());
}

// -------------------------------------------------------------------------- //

JsValueRef CHAKRA_CALLBACK
ScriptColorConstructor([[maybe_unused]] JsValueRef callee,
                       bool isConstruct,
                       JsValueRef* arguments,
                       u16 argumentCount,
                       [[maybe_unused]] void* callbackState)
{
  // Check that input is correct
  DIB_ASSERT(isConstruct, "Color constructor must be a construct call");
  if (argumentCount != 4 && argumentCount != 5) {
    DLOG_ERROR("Color constructor expects 3 or 4 arguments (Alpha channel "
               "defaults to 1.0)");
    return JS_INVALID_REFERENCE;
  }

  // Parse color values
  f32 values[4];
  values[3] = 1.0f;
  for (u16 i = 1; i < argumentCount; i++) {
    values[i - 1] = GetF32(arguments[i]);
  }

  // Create object
  graphics::Color* color =
    new graphics::Color(values[0], values[1], values[2], values[3]);
  JsValueRef object = CreateExternalObject(
    color, [](void* data) { delete static_cast<graphics::Color*>(data); });

  // Set prototype and return
  JsSetPrototype(object, COLOR_PROTOTYPE);
  return object;
}

// -------------------------------------------------------------------------- //

/** Expose the color class **/
void
ExposeColorClass(Environment& environment)
{
  // Create color constructor
  JsValueRef global = GetGlobal();
  JsValueRef constructor = CreateFunction(ScriptColorConstructor);
  SetProperty(global, "Color", constructor);

  // Set prototype and member functions
  COLOR_PROTOTYPE = CreateObject();
  SetProperty(COLOR_PROTOTYPE, "getRed", CreateFunction(ScriptColorGetRed));
  SetProperty(COLOR_PROTOTYPE, "getGreen", CreateFunction(ScriptColorGetGreen));
  SetProperty(COLOR_PROTOTYPE, "getBlue", CreateFunction(ScriptColorGetBlue));
  SetProperty(COLOR_PROTOTYPE, "getAlpha", CreateFunction(ScriptColorGetAlpha));
  SetProperty(constructor, "prototype", COLOR_PROTOTYPE);
}

}

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::script {

void
ExposeGraphics(Environment& environment)
{
  ExposeColorClass(environment);
}

}
