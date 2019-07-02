#include "game/mods/script/expose_world.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>

#include "core/assert.hpp"
#include "script/util.hpp"

// ========================================================================== //
// World
// ========================================================================== //

namespace dib::game {

/** World JavaScript prototype **/
static JsValueRef WORLD_PROTOTYPE = JS_INVALID_REFERENCE;

// -------------------------------------------------------------------------- //

/** World constructor (Not available to mods) **/
JsValueRef
ScriptWorldConstructor([[maybe_unused]] JsValueRef callee,
                       [[maybe_unused]] bool isConstruct,
                       [[maybe_unused]] JsValueRef* arguments,
                       [[maybe_unused]] u16 argumentCount,
                       [[maybe_unused]] void* callbackState)
{
  // Assert initial conditions
  DIB_ASSERT(isConstruct, "'World::constructor' is a constructor call");

  DLOG_WARNING("'World::constructor' is not available. Instead rely on "
               "callbacks where the 'World' object is given");
  return script::CreateUndefined();
}

}

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::game {

void
ExposeWorld()
{
  JsValueRef global = script::GetGlobal();
  JsValueRef constructor = script::CreateFunction(ScriptWorldConstructor);
  script::SetProperty(global, "World", constructor);

  WORLD_PROTOTYPE = script::CreateObject();
}

}