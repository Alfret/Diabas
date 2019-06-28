#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/world.hpp"
#include "game/client/tile_atlas.hpp"
#include "graphics/renderer.hpp"

// ========================================================================== //
// TerrainRenderer Declaration
// ========================================================================== //

namespace dib::game {

/** World renderer **/
class WorldRenderer
{
public:
  /** Render terrain **/
  static void RenderWorld(graphics::Renderer& renderer,
                          TileAtlas& tileAtlas,
                          World& world,
                          const graphics::Camera& camera);
};

}