#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <array>

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
  /** Common terrain sizes **/
  enum class Size
  {
    /** Small world **/
    kSmall,
    /** Normal world size **/
    kNormal,
    /** Large world size **/
    kLarge,
    /** Huge world size **/
    kHuge
  };

  /** Enumeration of layers in the world **/
  enum class Layer
  {
    /** Tile layer **/
    kTile = 0,
    /** Wall layer **/
    kWall,
    /** Wiring layer **/
    kWiring
  };

private:
  /** Tile registry **/
  const TileRegistry& mTileRegistry;

  /** Width of terrain **/
  u32 mWidth;
  /** Height of terrain **/
  u32 mHeight;

  /** Layer: Tile **/
  TileRegistry::TileID* mLayerTile;
  /** Layer: Wall **/
  TileRegistry::WallID* mLayerWall;
  /** Layer: Wiring **/
  TileRegistry::WireID* mLayerWiring;

  /** Layer: Metadata **/
  u8* mLayerMetadata;

public:
  /** Construct a world of the specified dimensions **/
  Terrain(const TileRegistry& tileRegistry, u32 width, u32 height);

  /** Construct a world of the specified size **/
  Terrain(const TileRegistry& tileRegistry, Size size);

  /** Returns the tile at the specified location in the world **/
  Tile* GetTile(WorldPos pos) const;

  /** Returns the ID of the tile at the specified location in the world **/
  TileRegistry::TileID GetTileID(WorldPos pos) const;

  /** Sets the tile at the specified location in the world **/
  void SetTile(WorldPos pos, Tile* tile);

  /** Sets the tile at the specified location in the world given its ID **/
  void SetTileID(WorldPos pos, TileRegistry::TileID id);

  /** Returns the width of the terrain in number of tiles. Zero (0) is left **/
  u32 GetWidth() const { return mWidth; };

  /** Returns the height of the terrain in number of tiles. Zero (0) is
   * bottom **/
  u32 GetHeight() const { return mHeight; };

private:
  /** Initialize the terrain layers **/
  void InitTerrain();
};

}