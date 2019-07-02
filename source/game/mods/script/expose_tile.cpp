#include "game/mods/script/expose_tile.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>

#include "core/assert.hpp"
#include "script/util.hpp"
#include "game/tile/tile_registry.hpp"

// ========================================================================== //
// TileRegistry
// ========================================================================== //

namespace dib::game {}

// ========================================================================== //
// Tile
// ========================================================================== //

namespace dib::game {

/** 'Tile' prototype **/
static JsValueRef TILE_PROTOTYPE = JS_INVALID_REFERENCE;

// -------------------------------------------------------------------------- //

JsValueRef
ScriptTileConstructor([[maybe_unused]] JsValueRef callee,
                      bool isConstruct,
                      JsValueRef* arguments,
                      u16 argumentCount,
                      [[maybe_unused]] void* callbackState)
{
  // Check initial condition
  DIB_ASSERT(isConstruct, "'Tile::constructor' is a constructor call");
  if (argumentCount != 4) {
    DLOG_WARNING(
      "'Tile::constructor' expects three (3) arguments, however got {}",
      argumentCount - 1);
    return script::CreateUndefined();
  }

  // Retrieve arguments
  JsValueRef modObject = arguments[1];
  String modId = script::GetString(script::GetProperty(modObject, "id"));
  String path = script::GetString(arguments[2]);
  String translationKey = script::GetString(arguments[3]);

  // Create tile
  Tile* tile = new Tile(ResourcePath{ modId, Path{ path } }, translationKey);
  JsValueRef tileObject = script::CreateExternalObject(tile, [](void* _object) {
    Tile* _tile = static_cast<Tile*>(_object);
    delete _tile;
  });

  // Return tile
  JsSetPrototype(tileObject, script::GetPrototype(arguments[0]));
  return tileObject;
}

// -------------------------------------------------------------------------- //

JsValueRef
ScriptTileSetIsLightEmitter([[maybe_unused]] JsValueRef callee,
                            [[maybe_unused]] bool isConstruct,
                            [[maybe_unused]] JsValueRef* arguments,
                            [[maybe_unused]] u16 argumentCount,
                            [[maybe_unused]] void* callbackState)
{
  // Check initial condition
  DIB_ASSERT(!isConstruct,
             "'Tile::setIsLightEmitter' is not a constructor call");
  if (argumentCount != 2) {
    DLOG_WARNING(
      "'Tile::setIsLightEmitter' expects one (1) argument, however got {}",
      argumentCount - 1);
    return script::CreateUndefined();
  }

  // Retrieve argument
  bool isLightEmitter = script::GetBool(arguments[1]);

  // Retrieve tile
  Tile* tile = script::GetExternalData<Tile>(arguments[0]);
  tile->SetIsLightEmitter(isLightEmitter);

  // Return self
  return arguments[0];
}

// -------------------------------------------------------------------------- //

void
ExposeTileClass()
{
  JsValueRef global = script::GetGlobal();
  JsValueRef constructor = script::CreateFunction(ScriptTileConstructor);
  script::SetProperty(global, "Tile", constructor);
  TILE_PROTOTYPE = script::CreateObject();
  script::SetProperty(TILE_PROTOTYPE,
                      "setIsLightEmitter",
                      script::CreateFunction(ScriptTileSetIsLightEmitter));
  script::SetProperty(constructor, "prototype", TILE_PROTOTYPE);
}

}

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::game {

void
ExposeTile()
{
  ExposeTileClass();
}

}