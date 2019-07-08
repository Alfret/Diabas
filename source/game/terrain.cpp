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

Terrain::Terrain(const TileRegistry& tileRegistry,
                 World& world,
                 u32 width,
                 u32 height)
  : mTileRegistry(tileRegistry)
  , mWorld(world)
  , mWidth(width)
  , mHeight(height)
{
  // Initialize
  InitTerrain();
}

// -------------------------------------------------------------------------- //

Terrain::Terrain(const TileRegistry& tileRegistry,
                 World& world,
                 Terrain::Size size)
  : mTileRegistry(tileRegistry)
  , mWorld(world)
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
  return (mTerrainCells + mWidth * pos.Y() + pos.X())->tile;
}

// -------------------------------------------------------------------------- //

bool
Terrain::SetTile(WorldPos pos, Tile* tile)
{
  return SetTile(pos, mTileRegistry.GetTileID(tile));
}

// -------------------------------------------------------------------------- //

bool
Terrain::SetTile(WorldPos pos, TileRegistry::TileID id)
{
  return SetTileAdvanced(pos, id);
}

// -------------------------------------------------------------------------- //

void
Terrain::GenSetTile(WorldPos pos, Tile* tile, bool updateNeighbours)
{
  GenSetTile(pos, mTileRegistry.GetTileID(tile), updateNeighbours);
}

// -------------------------------------------------------------------------- //

void
Terrain::GenSetTile(WorldPos pos,
                    TileRegistry::TileID id,
                    bool updateNeighbours)
{
  Cell& cell = GetCell(pos);
  Tile* tile = mTileRegistry.GetTile(cell.tile);
  tile->OnDestroyed(mWorld, pos);

  tile = mTileRegistry.GetTile(id);
  cell.tile = id;
  tile->OnPlaced(mWorld, pos);
  UpdateCachedIndices(pos, updateNeighbours);
}

// -------------------------------------------------------------------------- //

void
Terrain::RemoveTile(WorldPos pos, Tile* replacementTile)
{
  RemoveTile(pos, mTileRegistry.GetTileID(replacementTile));
}

// -------------------------------------------------------------------------- //

void
Terrain::RemoveTile(WorldPos pos, TileRegistry::TileID replacementId)
{
  SetTileAdvanced(pos, replacementId, true);
}

// -------------------------------------------------------------------------- //

void
Terrain::ReCacheResourceIndices()
{
  for (u32 y = 0; y < mHeight; y++) {
    for (u32 x = 0; x < mWidth; x++) {
      WorldPos pos{ x, y };
      Tile* tile = GetTile(pos);
      Cell& cell = GetCell(pos);
      cell.cachedTileSubResource = tile->GetResourceIndex(mWorld, pos);
    }
  }
}

// -------------------------------------------------------------------------- //

bool
Terrain::IsValidPosition(WorldPos pos)
{
  return pos.X() >= 0 && pos.X() < mWidth && pos.Y() >= 0 && pos.Y() < mHeight;
}

// -------------------------------------------------------------------------- //

u8
Terrain::GetMetadata(WorldPos pos)
{
  return GetCell(pos).metadata;
}

// -------------------------------------------------------------------------- //

void
Terrain::SetMetadata(WorldPos pos, u8 metadata)
{
  GetCell(pos).metadata = metadata;
}

// -------------------------------------------------------------------------- //

Terrain::Cell&
Terrain::GetCell(WorldPos pos)
{
  return *(mTerrainCells + mWidth * pos.Y() + pos.X());
}

// -------------------------------------------------------------------------- //

void
Terrain::InitTerrain()
{
  mTerrainCells = new Cell[mWidth * mHeight];
  memset(mTerrainCells, 0, sizeof(Cell) * mWidth * mHeight);
}

// -------------------------------------------------------------------------- //

bool
Terrain::SetTileAdvanced(WorldPos pos,
                         TileRegistry::TileID id,
                         bool ignoreReplaceCheck,
                         bool updateNeighbour)
{
  Cell& cell = GetCell(pos);

  // Notify old tile
  Tile* tile = mTileRegistry.GetTile(cell.tile);
  if (!ignoreReplaceCheck && !tile->CanBeReplaced(mWorld, pos)) {
    return false;
  }
  if (tile->IsMultiTile(mWorld, pos)) {
    const bool success = tile->KillMultiTile(mWorld, pos);
    if (!success) {
      DLOG_WARNING("Failed to destroy multi-tile structure");
      return false;
    }
  }
  tile->OnDestroyed(mWorld, pos);

  // Set new tile
  tile = mTileRegistry.GetTile(id);
  u32 oldId = cell.tile;
  cell.tile = id;
  if (tile->IsMultiTile(mWorld, pos)) {
    if (!tile->PlaceMultiTile(mWorld, pos)) {
      cell.tile = oldId;
      return false;
    }
  }
  tile->OnPlaced(mWorld, pos);
  UpdateCachedIndices(pos, updateNeighbour);
  return true;
}

// -------------------------------------------------------------------------- //

void
Terrain::SafeNeighbourUpdate(WorldPos pos)
{
  if (IsValidPosition(pos)) {
    Tile* tile = GetTile(pos);
    tile->OnNeighbourChange(mWorld, pos);
    Cell& cell = GetCell(pos);
    cell.cachedTileSubResource = tile->GetResourceIndex(mWorld, pos);
  }
}

// -------------------------------------------------------------------------- //

void
Terrain::UpdateCachedIndices(WorldPos pos, bool updateNeighbours)
{
  // Update center tile
  Tile* centerTile = GetTile(pos);
  Cell& centerCell = GetCell(pos);
  centerCell.cachedTileSubResource = centerTile->GetResourceIndex(mWorld, pos);

  // Update neighbours
  if (updateNeighbours) {
    SafeNeighbourUpdate(pos.TopLeft());
    SafeNeighbourUpdate(pos.Top());
    SafeNeighbourUpdate(pos.TopRight());
    SafeNeighbourUpdate(pos.Left());
    SafeNeighbourUpdate(pos.Right());
    SafeNeighbourUpdate(pos.BottomLeft());
    SafeNeighbourUpdate(pos.Bottom());
    SafeNeighbourUpdate(pos.BottomRight());
  }
}

}