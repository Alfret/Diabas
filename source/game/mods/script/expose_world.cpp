#include "game/mods/script/expose_world.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>

#include "core/assert.hpp"
#include "script/util.hpp"
#include "game/world.hpp"

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

// -------------------------------------------------------------------------- //

/** World constructor (Not available to mods) **/
JsValueRef
ScriptWorldSetTile([[maybe_unused]] JsValueRef callee,
                   [[maybe_unused]] bool isConstruct,
                   [[maybe_unused]] JsValueRef* arguments,
                   [[maybe_unused]] u16 argumentCount,
                   [[maybe_unused]] void* callbackState)
{
  // Assert initial conditions
  DIB_ASSERT(!isConstruct, "'World::setTile' is not a constructor call");
  if (argumentCount != 3 && argumentCount != 4) {
    DLOG_WARNING(
      "'World::setTile' expects two (2) or three (3) arguments, however got {}",
      argumentCount - 1);
    return script::CreateUndefined();
  }

  // Retrieve arguments
  auto world = script::GetExternalData<World>(arguments[0]);
  WorldPos pos;
  if (argumentCount == 3) {
    JsValueRef worldPosObject = arguments[1];
    pos.X() = script::GetU32(script::GetProperty(worldPosObject, "_x"));
    pos.Y() = script::GetU32(script::GetProperty(worldPosObject, "_y"));
  } else {
    pos.X() = script::GetU32(arguments[1]);
    pos.Y() = script::GetU32(arguments[2]);
  }
  auto tile = script::GetExternalData<Tile>(arguments[argumentCount - 1]);

  // Set tile
  world->GetTerrain().SetTile(pos, tile);

  return script::CreateUndefined();
}

// -------------------------------------------------------------------------- //

}

// ========================================================================== //
// WorldPos
// ========================================================================== //

namespace dib::game {

/** 'WorldPos' JavaScript prototype **/
static JsValueRef WORLD_POS_PROTOTYPE = JS_INVALID_REFERENCE;

// -------------------------------------------------------------------------- //

/** WorldPos constructor **/
JsValueRef
ScriptWorldPosConstructor([[maybe_unused]] JsValueRef callee,
                          bool isConstruct,
                          JsValueRef* arguments,
                          u16 argumentCount,
                          [[maybe_unused]] void* callbackState)
{
  // Assert initial conditions
  DIB_ASSERT(isConstruct, "'WorldPos::constructor' is a constructor call");
  if (argumentCount <= 1 || argumentCount > 3) {
    DLOG_WARNING("'WorldPos::constructor' expects zero (0) to two (2) "
                 "arguments, however got {}",
                 argumentCount - 1);
    return script::CreateUndefined();
  }

  // Create pos object
  JsValueRef posObject = script::CreateObject();
  JsSetPrototype(posObject, WORLD_POS_PROTOTYPE);

  // Setup position
  if (argumentCount == 3) {
    script::SetProperty(posObject, "_x", arguments[1]);
    script::SetProperty(posObject, "_y", arguments[2]);
  } else if (argumentCount == 2) {
    script::SetProperty(posObject, "_x", arguments[1]);
    script::SetProperty(posObject, "_y", script::CreateValue(0));
  } else {
    script::SetProperty(posObject, "_x", script::CreateValue(0));
    script::SetProperty(posObject, "_y", script::CreateValue(0));
  }

  return posObject;
}

// -------------------------------------------------------------------------- //

/** WorldPos constructor **/
JsValueRef
ScriptWorldPosGetX([[maybe_unused]] JsValueRef callee,
                   bool isConstruct,
                   JsValueRef* arguments,
                   u16 argumentCount,
                   [[maybe_unused]] void* callbackState)
{
  // Assert initial conditions
  DIB_ASSERT(!isConstruct, "'WorldPos::getX' is not a constructor call");
  if (argumentCount != 1) {
    DLOG_WARNING("'WorldPos::getX' expects zero (0) arguments, however got {}",
                 argumentCount - 1);
    return script::CreateUndefined();
  }

  // Return x position
  return script::GetProperty(arguments[0], "_x");
}

// -------------------------------------------------------------------------- //

/** WorldPos constructor **/
JsValueRef
ScriptWorldPosGetY([[maybe_unused]] JsValueRef callee,
                   bool isConstruct,
                   JsValueRef* arguments,
                   u16 argumentCount,
                   [[maybe_unused]] void* callbackState)
{
  // Assert initial conditions
  DIB_ASSERT(!isConstruct, "'WorldPos::getY' is not a constructor call");
  if (argumentCount != 1) {
    DLOG_WARNING("'WorldPos::getY' expects zero (0) arguments, however got {}",
                 argumentCount - 1);
    return script::CreateUndefined();
  }

  // Return x position
  return script::GetProperty(arguments[0], "_y");
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

  JsValueRef worldConstructor = script::CreateFunction(ScriptWorldConstructor);
  script::SetProperty(global, "World", worldConstructor);
  WORLD_PROTOTYPE = script::CreateObject();
  script::SetProperty(
    WORLD_PROTOTYPE, "setTile", script::CreateFunction(ScriptWorldSetTile));
  JsSetPrototype(worldConstructor, WORLD_PROTOTYPE);

  JsValueRef worldPosConstructor =
    script::CreateFunction(ScriptWorldPosConstructor);
  script::SetProperty(global, "WorldPos", worldPosConstructor);
  WORLD_POS_PROTOTYPE = script::CreateObject();
  script::SetProperty(
    WORLD_POS_PROTOTYPE, "getX", script::CreateFunction(ScriptWorldPosGetX));
  script::SetProperty(
    WORLD_POS_PROTOTYPE, "getY", script::CreateFunction(ScriptWorldPosGetY));
  JsSetPrototype(worldPosConstructor, WORLD_POS_PROTOTYPE);
}

// -------------------------------------------------------------------------- //

JsValueRef
CreateScriptWorldObject(World* world)
{
  JsValueRef object = script::CreateExternalObject(world);
  JsSetPrototype(object, WORLD_PROTOTYPE);
  return object;
}

}