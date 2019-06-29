#include "game/tile/tile_manager.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/tile/tile_pattern.hpp"

// ========================================================================== //
// TileManager Implementation
// ========================================================================== //

namespace dib::game {

TileManager::TileManager()
{
  RegisterBuiltinTiles();
}

// -------------------------------------------------------------------------- //

TileManager::Result
TileManager::RegisterTile(const String& modId,
                          const String& tileId,
                          const std::shared_ptr<Tile>& tile)
{
  // Check that ID only contain valid characters
  if (tileId.IndexOf('$') != -1) {
    return Result::kInvalidTileId;
  }

  // Store tile
  String identifier = modId + "$" + tileId;
  if (mTileMap.count(identifier) > 0) {
    return Result::kTileExists;
  }
  mTiles.push_back(TileEntry{ tile, identifier });
  Tile::ID numId = mTiles.size() - 1;
  mTileMap[identifier] = numId;
  tile->mID = numId;
  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

std::shared_ptr<Tile>
TileManager::GetTile(const String& id)
{
  auto it = mTileMap.find(id);
  if (it != mTileMap.end()) {
    return mTiles[it->second].tile;
  }
  return std::shared_ptr<Tile>(nullptr);
}

// -------------------------------------------------------------------------- //

std::shared_ptr<Tile>
TileManager::GetTile(const String& modId, const String& tileId)
{
  String identifier = modId + "$" + tileId;
  return GetTile(identifier);
}

// -------------------------------------------------------------------------- //

std::shared_ptr<Tile>
TileManager::GetTile(Tile::ID id)
{
  return mTiles[id].tile;
}

// -------------------------------------------------------------------------- //

void
TileManager::RegisterBuiltinTiles()
{
  std::shared_ptr<Tile> tile = std::make_shared<Tile>(
    ResourcePath{ ResourceType::kTexture, Path{ "./res/tiles/air_tile.tga" } });
  RegisterTile("builtin", "air", tile);

  tile = std::make_shared<TilePatternRug>(ResourcePath{
    ResourceType::kTextureTileRug, Path{ "./res/tiles/ghost_dirt.tga" } });
  RegisterTile("builtin", "ghost_dirt", tile);
}

}