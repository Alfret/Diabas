#include "game/terrain.hpp"

// ========================================================================== //
// Terrain Implementation
// ========================================================================== //

namespace dib::game {

Terrain::Terrain(World& world, TileManager& tileManager, u32 width, u32 height)
  : mWorld(world)
  , mTileManager(tileManager)
  , mWidth(width)
  , mHeight(height)
{
  mTerrainLayers[0] = new Tile::ID[mWidth * mHeight];
  memset(mTerrainLayers[0], 0, sizeof(Tile::ID) * mWidth * mHeight);

  mTerrainLayers[1] = new Tile::ID[mWidth * mHeight];
  memset(mTerrainLayers[1], 0, sizeof(Tile::ID) * mWidth * mHeight);

  mTerrainLayers[2] = new Tile::ID[mWidth * mHeight];
  memset(mTerrainLayers[2], 0, sizeof(Tile::ID) * mWidth * mHeight);
}

// -------------------------------------------------------------------------- //

void
Terrain::Generate()
{
  std::shared_ptr<Tile> tileAir = mTileManager.GetTile("builtin$air");
  std::shared_ptr<Tile> tileGrass = mTileManager.GetTile("core$grass");
  std::shared_ptr<Tile> tileDirt = mTileManager.GetTile("core$dirt");

  for (u32 y = 0; y < mHeight; y++) {
    for (u32 x = 0; x < mWidth; x++) {

      if (y > 10) {
        SetTile(tileAir, x, y, LAYER_FOREGROUND);
      } else if (y == 10) {
        SetTile(tileGrass, x, y, LAYER_FOREGROUND);
      } else {
        SetTile(tileDirt, x, y, LAYER_FOREGROUND);
      }
    }
  }
}

// -------------------------------------------------------------------------- //

std::shared_ptr<Tile>
Terrain::GetTile(u32 x, u32 y, u32 layer)
{
  Tile::ID id = *GetTerrainTileOffset(x, y, layer);
  return mTileManager.GetTile(id);
}

// -------------------------------------------------------------------------- //

void
Terrain::SetTile(const std::shared_ptr<Tile>& tile, u32 x, u32 y, u32 layer)
{
  *GetTerrainTileOffset(x, y, layer) = tile->GetID();
}

// -------------------------------------------------------------------------- //

void
Terrain::PickTile(const graphics::Camera& camera,
                  f64 mouseX,
                  f64 mouseY,
                  u32& tileX,
                  u32& tileY)
{
  tileX =
    std::floor((camera.GetPosition().x + mouseX) / TileManager::TILE_SIZE);
  tileX = alflib::Clamp(tileX, 0u, mWidth);
  tileY =
    std::floor((camera.GetPosition().y + mouseY) / TileManager::TILE_SIZE);
  tileY = alflib::Clamp(tileY, 0u, mHeight);
}

// -------------------------------------------------------------------------- //

Tile::ID*
Terrain::GetTerrainTileOffset(u32 x, u32 y, u32 layer)
{
  return mTerrainLayers[layer] + (y * mWidth) + x;
}
}