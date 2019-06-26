#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/tile/tile.hpp"

// ========================================================================== //
// TileManager Declaration
// ========================================================================== //

namespace dib::game {

/** Tile manager **/
class TileManager
{
public:
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

private:
  /** Map of registered tiles **/
  std::unordered_map<String, std::shared_ptr<Tile>> mTiles;

public:
  TileManager();

  /** Register a tile **/
  Result RegisterTile(const String& modId,
                      const String& tileId,
                      std::shared_ptr<Tile> tile);

  /** Create the block atlas from all registered blocks **/
  void CreateAtlas();

  /** Returns a tile or nullptr if it was not previously registered **/
  std::shared_ptr<Tile> GetTile(const String& modId, const String& tileId);

  /** Returns the map of tiles **/
  const std::unordered_map<String, std::shared_ptr<Tile>>& GetTiles() const
  {
    return mTiles;
  }
};

}