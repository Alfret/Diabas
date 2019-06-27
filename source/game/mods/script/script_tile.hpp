#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "script/env.hpp"
#include "game/tile/tile_manager.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::game {

/** Expose tile related things to scripts **/
void
ExposeScriptTile(script::Environment& environment);

// -------------------------------------------------------------------------- //

/** Create a JavaScript object that wraps a tile manager **/
JsValueRef
CreateTileManagerScriptObject(TileManager& tileManager);

}