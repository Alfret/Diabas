#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include "core/macros.hpp"
#include "graphics/camera.hpp"
#include "game/tile/tile_registry.hpp"

// ========================================================================== //
// Terrain Declaration
// ========================================================================== //

namespace dib::game {

/** Class that manages the terrain  **/
class Terrain
{
public:
  /** Enumeration of layers in the world **/
  enum class Layer
  {
    /** Foreground layer **/
    kForeground = 0,
    /** Background layer **/
    kBackground,
    /** Wiring layer **/
    kWiring
  };

private:
  /** Width of terrain **/
  u32 mWidth;
  /** Height of terrain **/
  u32 mHeight;

  /** Layer: Foreground **/
  TileRegistry::TileID* mLayerMain;
  /** Layer: Background **/
  TileRegistry::WallID* mLayerWall;
  /** Layer: Wiring **/
  TileRegistry::WireID* mLayerWiring;

  /** Layer: Data **/
  u8* mLayerData;

public:
  Terrain(u32 width, u32 height);

  /** Returns the width of the terrain in number of tiles. Zero (0) is left **/
  u32 GetWidth() const { return mWidth; };

  /** Returns the height of the terrain in number of tiles. Zero (0) is
   * bottom **/
  u32 GetHeight() const { return mHeight; };
};

}