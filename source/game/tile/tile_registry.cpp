#include "game/tile/tile_registry.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/assert.hpp"
#include "game/constants.hpp"
#include "tile_registry.hpp"

// ========================================================================== //
// TileRegistry Implementation
// ========================================================================== //

namespace dib::game {

TileRegistry::TileRegistry()
{
  // Register invalid tile
  mBuiltin.tileInvalid =
    new Tile(ResourcePath{ Path{ "./res/tiles/invalid.tga" } }, "invalid");
  mBuiltin.tileInvalid->SetIsDestructible(false);
  RegisterTile(CreateRegistryKey(BUILTIN_MOD_NAME, "invalid"),
               mBuiltin.tileInvalid);

  // Register missing tile
  mBuiltin.tileMissing =
    new Tile(ResourcePath{ Path{ "./res/tiles/missing.tga" } }, "missing");
  RegisterTile(CreateRegistryKey(BUILTIN_MOD_NAME, "missing"),
               mBuiltin.tileMissing);
}

// -------------------------------------------------------------------------- //

TileRegistry::~TileRegistry()
{
  delete mBuiltin.tileMissing;
  delete mBuiltin.tileInvalid;
}

// -------------------------------------------------------------------------- //

bool
TileRegistry::RegisterTile(const String& modId,
                           const String& registryKey,
                           Tile* tile)
{
  return RegisterTile(CreateRegistryKey(modId, registryKey), tile);
}

// -------------------------------------------------------------------------- //

bool
TileRegistry::RegisterTile(const String& registryKey, Tile* tile)
{
  DIB_ASSERT(tile != nullptr, "Cannot register null tile");

  // Make sure it's not already registered
  if (mTileRegistryMap.count(registryKey) > 0) {
    return false;
  }

  // Register tile with next ID
  mTiles.push_back(tile);
  const TileID id = mTiles.size() - 1;

  // Setup maps
  mTileRegistryMap[registryKey] = id;
  mTileMap[tile] = id;
  return true;
}

// -------------------------------------------------------------------------- //

Tile*
TileRegistry::GetTile(TileRegistry::TileID id) const
{
  return mTiles[id];
}

// -------------------------------------------------------------------------- //

Tile*
TileRegistry::GetTile(const String& registryKey) const
{
  return GetTile(GetTileID(registryKey));
}

// -------------------------------------------------------------------------- //

TileRegistry::TileID
TileRegistry::GetTileID(const String& registryKey) const
{
  return mTileRegistryMap.at(registryKey);
}

// -------------------------------------------------------------------------- //

TileRegistry::TileID
TileRegistry::GetTileID(Tile* tile) const
{
  return mTileMap.at(tile);
}

// -------------------------------------------------------------------------- //

String
TileRegistry::CreateRegistryKey(const String& mod, const String& key)
{
  return mod + "$" + key;
}

}