#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <tsl/robin_map.h>
#include "core/types.hpp"
#include "game/tile/tile.hpp"

// ========================================================================== //
// Macros
// ========================================================================== //

/** Macro for creating a TileRegistry key constant at compile-time **/
#define DIB_TILE_REGISTRY_KEY(mod, key) mod "$" key

// ========================================================================== //
// TileRegistry Declaration
// ========================================================================== //

namespace dib::game {

/** Tile registry that manages all the registration of tiles **/
class TileRegistry
{
public:
  /** Type for tile IDs **/
  using TileID = u16;

private:
  /** List of registered tiles. Indices in the array are their IDs **/
  std::vector<Tile*> mTiles;
  /** Map of tiles from their register-key to their IDs **/
  tsl::robin_map<String, TileID> mTileRegistryMap;
  /** Map of tiles from themselves to their IDs **/
  tsl::robin_map<Tile*, TileID> mTileMap;

  /** Builtin tiles **/
  struct
  {
    /** Invalid tile **/
    Tile* tileInvalid = nullptr;
    /** Missing tile **/
    Tile* tileMissing = nullptr;
  } mBuiltin;

public:
  /** Construct tile registry with only builtin tiles registered **/
  TileRegistry();

  /** Destruct **/
  ~TileRegistry();

  /** Register a tile with the registry. If the tile has already been registered
   * then the function returns false. This function will combine the modId and
   * the registryKey to create the actual key that is stored in the registry **/
  bool RegisterTile(const String& modId, const String& registryKey, Tile* tile);

  /** Register a tile with the registry. If the tile has already been registered
   * then the function returns false **/
  bool RegisterTile(const String& registryKey, Tile* tile);

  /** Returns the tile for a given ID **/
  [[nodiscard]] Tile* GetTile(TileID id) const;

  /** Returns the tile for a given registry key **/
  [[nodiscard]] Tile* GetTile(const String& registryKey) const;

  /** Returns the tile ID for a given registry key **/
  [[nodiscard]] TileID GetTileID(const String& registryKey) const;

  /** Returns the tile ID for a given tile **/
  [[nodiscard]] TileID GetTileID(Tile* tile) const;

  /** Returns the list of all registered tiles. Indices are the IDs of the
   * tile **/
  [[nodiscard]] const std::vector<Tile*>& GetTiles() const { return mTiles; }

  /** Returns the map of registered tiles. Keys are the registry names and the
   * value is the ID of the tile. This ID can then be used to index the vector
   * returned from 'GetTiles()'**/
  [[nodiscard]] const tsl::robin_map<String, TileID>& GetRegistryMap() const
  {
    return mTileRegistryMap;
  }

public:
  /** Returns the TileRegistry instance **/
  static TileRegistry& Instance();

  /** Create a registry key from the name of a mod and the mod-specific key **/
  static String CreateRegistryKey(const String& mod, const String& key);
};

}
