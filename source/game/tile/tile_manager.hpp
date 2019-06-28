#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <unordered_map>

#include "game/tile/tile.hpp"

// ========================================================================== //
// TileManager Declaration
// ========================================================================== //

namespace dib::game {

/** Tile manager **/
class TileManager
{
public:
  /** Size of a tile in pixels **/
  static constexpr u32 TILE_SIZE = 16;

  /** Tile-related result values **/
  enum class Result
  {
    /** Success **/
    kSuccess,
    /** Tile already exists **/
    kTileExists,
    /** Invalid tile id. Tile id:s cannot contain '$' **/
    kInvalidTileId
  };

  /** Entry in tile list **/
  struct TileEntry
  {
    std::shared_ptr<Tile> tile;
    String id;
  };

private:
  /** List of registered tiles, indices are their ID:s **/
  std::vector<TileEntry> mTiles;
  /** Map of registered tiles from ID to index **/
  std::unordered_map<String, Tile::ID> mTileMap;

public:
  TileManager();

  /** Register a tile **/
  Result RegisterTile(const String& modId,
                      const String& tileId,
                      const std::shared_ptr<Tile>& tile);

  /** Returns a tile or nullptr if it was not previously registered **/
  std::shared_ptr<Tile> GetTile(const String& id);

  /** Returns a tile or nullptr if it was not previously registered **/
  std::shared_ptr<Tile> GetTile(const String& modId, const String& tileId);

  /** Returns tile from it's numerical id **/
  std::shared_ptr<Tile> GetTile(Tile::ID id);

  /** Returns the list of tiles **/
  const std::vector<TileEntry>& GetTiles() const { return mTiles; }

  /** Return tile string id from numeric id **/
  const String& GetTileID(Tile::ID id) const { return mTiles[id].id; }

  /** Returns the numerical ID from the string id **/
  Tile::ID GetNumericIdFromStringId(const String& id) const
  {
    return mTileMap.at(id);
  }

private:
  /** Register builtin tiles **/
  void RegisterBuiltinTiles();
};

}
