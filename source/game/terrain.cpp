#include "game/terrain.hpp"

// ========================================================================== //
// Terrain Implementation
// ========================================================================== //

namespace dib::game {

Terrain::Terrain(TileManager& tileManager, u32 width, u32 height)
  : mTileManager(tileManager)
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
Terrain::Draw(graphics::Renderer& renderer, graphics::Camera& camera)
{
  renderer.GetSpriteBatch();
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

Tile::ID*
Terrain::GetTerrainTileOffset(u32 x, u32 y, u32 layer)
{
  return mTerrainLayers[layer] + (y * mWidth) + x;
}
}