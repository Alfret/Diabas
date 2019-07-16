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

Terrain::Terrain(World* world, u32 width, u32 height)
  : mWorld(world)
  , mWidth(width)
  , mHeight(height)
{
  // Initialize
  InitTerrain();
}

// -------------------------------------------------------------------------- //

Terrain::Terrain(World* world, Terrain::Size size)
  : mWorld(world)
{
  // Determine size
  switch (size) {
    case Size::kTiny: {
      mWidth = 2100;
      mHeight = 600;
      break;
    }
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

Terrain::Terrain(Terrain&& other) noexcept
  : mWorld(other.mWorld)
  , mWidth(other.mWidth)
  , mHeight(other.mHeight)
  , mTerrainCells(other.mTerrainCells)
  , mChangeListeners(std::move(other.mChangeListeners))
{
  other.mTerrainCells = nullptr;
}

// -------------------------------------------------------------------------- //

Terrain::~Terrain()
{
  delete mTerrainCells;
}

// -------------------------------------------------------------------------- //

Terrain&
Terrain::operator=(Terrain&& other) noexcept
{
  if (this != &other) {
    mWorld = other.mWorld;
    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mTerrainCells = other.mTerrainCells;
    mChangeListeners = other.mChangeListeners;
    other.mTerrainCells = nullptr;
  }
  return *this;
}

// -------------------------------------------------------------------------- //

Tile*
Terrain::GetTile(WorldPos pos) const
{
  return TileRegistry::Instance().GetTile(GetTileID(pos));
}

// -------------------------------------------------------------------------- //

TileRegistry::TileID
Terrain::GetTileID(WorldPos pos) const
{
  return (mTerrainCells + mWidth * pos.Y() + pos.X())->tile;
}

// -------------------------------------------------------------------------- //

Wall*
Terrain::GetWall(WorldPos pos) const
{
  return WallRegistry::Instance().GetWall(GetWallID(pos));
}

// -------------------------------------------------------------------------- //

WallRegistry::WallID
Terrain::GetWallID(WorldPos pos) const
{
  return (mTerrainCells + mWidth * pos.Y() + pos.X())->wall;
}

// -------------------------------------------------------------------------- //

bool
Terrain::SetTile(WorldPos pos, Tile* tile)
{
  return SetTile(pos, TileRegistry::Instance().GetTileID(tile));
}

// -------------------------------------------------------------------------- //

bool
Terrain::SetTile(WorldPos pos, TileRegistry::TileID id)
{
  return SetTileAdvanced(pos, id);
}

// -------------------------------------------------------------------------- //

bool
Terrain::SetWall(WorldPos pos, Wall* wall)
{
  return SetWall(pos, WallRegistry::Instance().GetWallID(wall));
}

// -------------------------------------------------------------------------- //

bool
Terrain::SetWall(WorldPos pos, WallRegistry::WallID id)
{
  return SetWallAdvanced(pos, id);
}

// -------------------------------------------------------------------------- //

void
Terrain::GenSetTile(WorldPos pos, Tile* tile, bool updateNeighbours)
{
  GenSetTile(pos, TileRegistry::Instance().GetTileID(tile), updateNeighbours);
}

// -------------------------------------------------------------------------- //

void
Terrain::GenSetTile(WorldPos pos,
                    TileRegistry::TileID id,
                    bool updateNeighbours)
{
  Cell& cell = GetCell(pos);
  Tile* tile = TileRegistry::Instance().GetTile(cell.tile);
  tile->OnDestroyed(*mWorld, pos);

  tile = TileRegistry::Instance().GetTile(id);
  cell.tile = id;
  tile->OnPlaced(*mWorld, pos);
  UpdateCachedTileIndices(pos, updateNeighbours);
}

// -------------------------------------------------------------------------- //

void
Terrain::GenSetWall(WorldPos pos, Wall* wall, bool updateNeighbours)
{
  GenSetWall(pos, WallRegistry::Instance().GetWallID(wall), updateNeighbours);
}

// -------------------------------------------------------------------------- //

void
Terrain::GenSetWall(WorldPos pos,
                    WallRegistry::WallID id,
                    bool updateNeighbours)
{
  Cell& cell = GetCell(pos);
  Wall* wall = WallRegistry::Instance().GetWall(cell.wall);
  wall->OnDestroyed(*mWorld, pos);

  wall = WallRegistry::Instance().GetWall(id);
  cell.wall = id;
  wall->OnPlaced(*mWorld, pos);
  UpdateCachedWallIndices(pos, updateNeighbours);
}

// -------------------------------------------------------------------------- //

void
Terrain::RemoveTile(WorldPos pos, Tile* replacementTile)
{
  RemoveTile(pos, TileRegistry::Instance().GetTileID(replacementTile));
}

// -------------------------------------------------------------------------- //

void
Terrain::RemoveTile(WorldPos pos, TileRegistry::TileID replacementId)
{
  SetTileAdvanced(pos, replacementId, true);
}

// -------------------------------------------------------------------------- //

void
Terrain::Resize(u32 width, u32 height)
{
  delete mTerrainCells;
  mWidth = width;
  mHeight = height;
  mTerrainCells = new Cell[mWidth * mHeight];
  memset(mTerrainCells, 0, sizeof(Cell) * mWidth * mHeight);
  for (auto& listener : mChangeListeners) {
    listener->OnResize(width, height);
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

void
Terrain::RegisterChangeListener(Terrain::ChangeListener* changeListener)
{
  mChangeListeners.push_back(changeListener);
}

// -------------------------------------------------------------------------- //

void
Terrain::UnregisterChangeListener(Terrain::ChangeListener* changeListener)
{
  mChangeListeners.erase(std::remove(mChangeListeners.begin(),
                                     mChangeListeners.end(),
                                     changeListener),
                         mChangeListeners.end());
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
  Tile* tile = TileRegistry::Instance().GetTile(cell.tile);
  if (!ignoreReplaceCheck && !tile->CanBeReplaced(*mWorld, pos)) {
    return false;
  }
  if (tile->IsMultiTile(*mWorld, pos)) {
    const bool success = tile->KillMultiTile(*mWorld, pos);
    if (!success) {
      DLOG_WARNING("Failed to destroy multi-tile structure");
      return false;
    }
  }
  tile->OnDestroyed(*mWorld, pos);

  // Set new tile
  tile = TileRegistry::Instance().GetTile(id);
  u32 oldId = cell.tile;
  cell.tile = id;
  if (tile->IsMultiTile(*mWorld, pos)) {
    if (!tile->PlaceMultiTile(*mWorld, pos)) {
      cell.tile = oldId;
      return false;
    }
  }
  tile->OnPlaced(*mWorld, pos);
  UpdateCachedTileIndices(pos, updateNeighbour);
  return true;
}

// -------------------------------------------------------------------------- //

void
Terrain::SafeNeighbourTileUpdate(WorldPos pos)
{
  if (IsValidPosition(pos)) {
    Tile* tile = GetTile(pos);
    tile->OnNeighbourChange(*mWorld, pos);
    for (auto& listener : mChangeListeners) {
      listener->OnTileChanged(pos);
    }
  }
}

// -------------------------------------------------------------------------- //

void
Terrain::UpdateCachedTileIndices(WorldPos pos, bool updateNeighbours)
{
  // Update center tile
  for (auto& listener : mChangeListeners) {
    listener->OnTileChanged(pos);
  }

  // Update neighbours
  if (updateNeighbours) {
    SafeNeighbourTileUpdate(pos.TopLeft());
    SafeNeighbourTileUpdate(pos.Top());
    SafeNeighbourTileUpdate(pos.TopRight());
    SafeNeighbourTileUpdate(pos.Left());
    SafeNeighbourTileUpdate(pos.Right());
    SafeNeighbourTileUpdate(pos.BottomLeft());
    SafeNeighbourTileUpdate(pos.Bottom());
    SafeNeighbourTileUpdate(pos.BottomRight());
  }
}

// -------------------------------------------------------------------------- //

bool
Terrain::SetWallAdvanced(WorldPos pos,
                         WallRegistry::WallID id,
                         bool ignoreReplaceCheck,
                         bool updateNeighbour)
{
  Cell& cell = GetCell(pos);

  // Notify old tile
  Wall* wall = WallRegistry::Instance().GetWall(cell.wall);
  if (!ignoreReplaceCheck && !wall->CanBeReplaced(*mWorld, pos)) {
    return false;
  }
  wall->OnDestroyed(*mWorld, pos);

  // Set new tile
  wall = WallRegistry::Instance().GetWall(id);
  cell.wall = id;
  wall->OnPlaced(*mWorld, pos);
  UpdateCachedWallIndices(pos, updateNeighbour);
  return true;
}

// -------------------------------------------------------------------------- //

void
Terrain::SafeNeighbourWallUpdate(WorldPos pos)
{
  if (IsValidPosition(pos)) {
    Wall* wall = GetWall(pos);
    wall->OnNeighbourChange(*mWorld, pos);
    for (auto& listener : mChangeListeners) {
      listener->OnWallChanged(pos);
    }
  }
}

// -------------------------------------------------------------------------- //

void
Terrain::UpdateCachedWallIndices(WorldPos pos, bool updateNeighbours)
{
  // Update center tile
  for (auto& listener : mChangeListeners) {
    listener->OnWallChanged(pos);
  }

  // Update neighbours
  if (updateNeighbours) {
    SafeNeighbourWallUpdate(pos.TopLeft());
    SafeNeighbourWallUpdate(pos.Top());
    SafeNeighbourWallUpdate(pos.TopRight());
    SafeNeighbourWallUpdate(pos.Left());
    SafeNeighbourWallUpdate(pos.Right());
    SafeNeighbourWallUpdate(pos.BottomLeft());
    SafeNeighbourWallUpdate(pos.Bottom());
    SafeNeighbourWallUpdate(pos.BottomRight());
  }
}

}