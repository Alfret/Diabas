#include "game/mods/script/script_tile.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>
#include "core/assert.hpp"
#include "script/util.hpp"
#include "game/tile/tile_manager.hpp"
#include "game/tile/tile.hpp"

// ========================================================================== //
// Structures
// ========================================================================== //

namespace dib::game {

/** Script 'Tile' wrapper **/
struct ScriptTile
{
  /** Mod that created tile **/
  JsValueRef mod;
  /** Pointer to tile **/
  std::shared_ptr<Tile> pointer;
};

}

// ========================================================================== //
// TileManager Functions
// ========================================================================== //

namespace dib::game {

/** Prototype of TileManager class in scripts **/
static JsValueRef TILE_MANAGER_PROTOTYPE = JS_INVALID_REFERENCE;

// -------------------------------------------------------------------------- //

/** TileManager constructor, cannot be called by user **/
JsValueRef CHAKRA_CALLBACK
ScriptTileManagerConstructor([[maybe_unused]] JsValueRef callee,
                             [[maybe_unused]] bool isConstruct,
                             [[maybe_unused]] JsValueRef* arguments,
                             [[maybe_unused]] u16 argumentCount,
                             [[maybe_unused]] void* callbackState)
{
  // Warn user
  DLOG_WARNING(
    "'TileManager::constructor()' is not available to scripts. Instead "
    "override the 'Mod::onRegisterTiles()' function that receives a "
    "'TileManager' that should be used for registering any tiles");
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

/** TileManager constructor, cannot be called by user **/
JsValueRef CHAKRA_CALLBACK
ScriptTileManagerRegisterTile([[maybe_unused]] JsValueRef callee,
                              [[maybe_unused]] bool isConstruct,
                              [[maybe_unused]] JsValueRef* arguments,
                              [[maybe_unused]] u16 argumentCount,
                              [[maybe_unused]] void* callbackState)
{
  // Check initial condition
  DIB_ASSERT(!isConstruct,
             "'TileManager::registerTile()' is not a constructor call");
  if (argumentCount != 3) {
    DLOG_WARNING(
      "'TileManager::registerTile()' expects two (2) argument, however got {}",
      argumentCount - 1);
    return script::CreateValue(false);
  }

  // Retrieve arguments
  String tileId = script::GetString(arguments[1]);
  auto tile = script::GetExternalData<ScriptTile>(arguments[2]);
  auto tileMgr = script::GetExternalData<TileManager>(arguments[0]);

  // Register tile
  TileManager::Result result = tileMgr->RegisterTile(
    script::GetString(script::GetProperty(tile->mod, "id")),
    tileId,
    tile->pointer);
  return script::CreateValue(result != TileManager::Result::kSuccess);
}

// -------------------------------------------------------------------------- //

void
ExposeTileManagerScript()
{
  JsValueRef global = script::GetGlobal();
  JsValueRef constructor = script::CreateFunction(ScriptTileManagerConstructor);
  script::SetProperty(global, "TileManager", constructor);

  TILE_MANAGER_PROTOTYPE = script::CreateObject();
  script::SetProperty(TILE_MANAGER_PROTOTYPE,
                      "registerTile",
                      script::CreateFunction(ScriptTileManagerRegisterTile));
  script::SetProperty(constructor, "prototype", TILE_MANAGER_PROTOTYPE);
}
}

// ========================================================================== //
// Tile Functions
// ========================================================================== //

namespace dib::game {

/** Prototype of Tile class in scripts **/
static JsValueRef TILE_PROTOTYPE = JS_INVALID_REFERENCE;

// -------------------------------------------------------------------------- //

/** TileManager constructor, cannot be called by user **/
JsValueRef CHAKRA_CALLBACK
ScriptTileConstructor([[maybe_unused]] JsValueRef callee,
                      [[maybe_unused]] bool isConstruct,
                      [[maybe_unused]] JsValueRef* arguments,
                      [[maybe_unused]] u16 argumentCount,
                      [[maybe_unused]] void* callbackState)
{
  // Warn user
  DIB_ASSERT(isConstruct, "'Tile::constructor()' is a constructor call");
  if (argumentCount != 3) {
    DLOG_WARNING(
      "'Tile::constructor()' expected two (2) argument, however got {}",
      argumentCount - 1);
    return script::CreateUndefined();
  }

  // Retrieve arguments
  JsValueRef modObject = arguments[1];
  String modId = script::GetString(script::GetProperty(modObject, "id"));
  String tilePath = script::GetString(arguments[2]);

  // Setup tile data
  auto scriptTile = new ScriptTile;
  Tile* tile = new Tile(ResourcePath(ResourceType::kTexture, modId, tilePath));
  scriptTile->pointer = std::shared_ptr<Tile>(tile);
  scriptTile->mod = modObject;

  // Create 'Tile' JavaScript object
  JsValueRef object = script::CreateExternalObject(scriptTile, [](void* _data) {
    auto data = static_cast<ScriptTile*>(_data);
    delete data;
  });
  JsSetPrototype(object, script::GetPrototype(arguments[0]));
  return object;
}

// -------------------------------------------------------------------------- //

void
ExposeTileScript()
{
  JsValueRef global = script::GetGlobal();
  JsValueRef constructor = script::CreateFunction(ScriptTileConstructor);
  script::SetProperty(global, "Tile", constructor);

  TILE_PROTOTYPE = script::CreateObject();
  // script::SetProperty(TILE_PROTOTYPE,
  //                    "",
  //                    script::CreateFunction(ScriptTileManagerRegisterTile));
  script::SetProperty(constructor, "prototype", TILE_MANAGER_PROTOTYPE);
}

}

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::game {

void
ExposeScriptTile(script::Environment& environment)
{
  ExposeTileManagerScript();
  ExposeTileScript();
}

// -------------------------------------------------------------------------- //

JsValueRef
CreateTileManagerScriptObject(TileManager& tileManager)
{
  JsValueRef object = script::CreateExternalObject(&tileManager);
  JsSetPrototype(object, TILE_MANAGER_PROTOTYPE);
  return object;
}

}