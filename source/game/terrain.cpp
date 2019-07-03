#include "game/terrain.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <random>

#include "game/world.hpp"

// ========================================================================== //
// Terrain Implementation
// ========================================================================== //

namespace dib::game {

Terrain::Terrain(const TileRegistry& tileRegistry, u32 width, u32 height)
  : mTileRegistry(tileRegistry)
  , mWidth(width)
  , mHeight(height)
{
  // Initialize
  InitTerrain();
}

// -------------------------------------------------------------------------- //

Terrain::Terrain(const TileRegistry& tileRegistry, Terrain::Size size)
  : mTileRegistry(tileRegistry)
{
  // Determine size
  switch (size) {
    case Size::kSmall: {
      mWidth = 4200;
      mHeight = 1200;
      break;
    }
    case Size::kLarge: {
      mWidth = 8400;
      mHeight = 2400;
      break;
    }
    case Size::kHuge: {
      mWidth = 16800;
      mHeight = 4800;
      break;
    }
    case Size::kNormal:
    default: {
      mWidth = 6400;
      mHeight = 1800;
      break;
    }
  }

  // Initialize
  InitTerrain();
}

// -------------------------------------------------------------------------- //

Tile*
Terrain::GetTile(WorldPos pos) const
{
  return mTileRegistry.GetTile(GetTileID(pos));
}

// -------------------------------------------------------------------------- //

TileRegistry::TileID
Terrain::GetTileID(WorldPos pos) const
{
  return *(mLayerTile + mWidth * pos.Y() + pos.X());
}

// -------------------------------------------------------------------------- //

void
Terrain::SetTile(WorldPos pos, Tile* tile)
{
  SetTile(pos, mTileRegistry.GetTileID(tile));
}

// -------------------------------------------------------------------------- //

void
Terrain::SetTile(WorldPos pos, TileRegistry::TileID id)
{
  *(mLayerTile + mWidth * pos.Y() + pos.X()) = id;
}

// -------------------------------------------------------------------------- //

void
Terrain::InitTerrain()
{
  mLayerTile = new TileRegistry::TileID[mWidth * mHeight];
  mLayerWall = new TileRegistry::WallID[mWidth * mHeight];
  mLayerWiring = new TileRegistry::WireID[mWidth * mHeight];
  mLayerMetadata = new u8[mWidth * mHeight];

  for (u32 y = 0; y < mHeight; y++) {
    for (u32 x = 0; x < mWidth; x++) {
      SetTile(WorldPos{ x, y }, 2);
    }
  }
}

}