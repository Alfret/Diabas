#include "game/tile/tile_manager.hpp"

// ========================================================================== //
// TileManager Implementation
// ========================================================================== //

namespace dib::game {

TileManager::TileManager() {}

// -------------------------------------------------------------------------- //

TileManager::Result
TileManager::RegisterTile(const String& modId,
                          const String& tileId,
                          std::shared_ptr<Tile> tile)
{
  // Check that ID only contain valid characters
  if (tileId.IndexOf('$') != -1) {
    return Result::kInvalidTileId;
  }

  // Store tile
  String identifier = modId + "$" + tileId;
  if (mTiles.count(identifier) > 0) {
    return Result::kTileExists;
  }
  mTiles[identifier] = tile;
  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

void
TileManager::CreateAtlas()
{}

// -------------------------------------------------------------------------- //

std::shared_ptr<Tile>
TileManager::GetTile(const String& modId, const String& tileId)
{
  String identifier = modId + "$" + tileId;
  auto it = mTiles.find(identifier);
  if (it != mTiles.end()) {
    return it->second;
  }
  return std::shared_ptr<Tile>(nullptr);
}

}