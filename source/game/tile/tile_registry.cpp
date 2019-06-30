#include "game/tile/tile_registry.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/constants.hpp"
#include "tile_registry.hpp"

// ========================================================================== //
// TileRegistry Implementation
// ========================================================================== //

namespace dib::game {

TileRegistry::TileRegistry()
{
  // Register air tile
  mBuiltin.tileAir =
    new Tile(ResourcePath{ Path{ "./res/tiles/air.tga" } }, "air");
  mBuiltin.tileAir->SetOpacity(0.0f)
    ->SetCollisionType(Tile::CollisionType::kNone)
    ->SetIsDestructible(false);
  RegisterTile(CreateRegistryKey(BUILTIN_MOD_NAME, "air"), mBuiltin.tileAir);
}

// -------------------------------------------------------------------------- //

TileRegistry::~TileRegistry()
{
  delete mBuiltin.tileAir;
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
  // Make sure it's not already registered
  if (mTileRegistryMap.count(registryKey) > 0) {
    return false;
  }

  // Register tile with next ID
  mTiles.push_back(tile);
  const TileID id = mTiles.size() - 1;

  // Set registry key
  mTileRegistryMap[registryKey] = id;
  return true;
}

// -------------------------------------------------------------------------- //

Tile*
TileRegistry::GetTile(TileRegistry::TileID id)
{
  return mTiles[id];
}

// -------------------------------------------------------------------------- //

Tile*
TileRegistry::GetTile(const String& registryKey)
{
  return GetTile(GetTileID(registryKey));
}

// -------------------------------------------------------------------------- //

TileRegistry::TileID
TileRegistry::GetTileID(const String& registryKey)
{
  return mTileRegistryMap[registryKey];
}

// -------------------------------------------------------------------------- //

String
TileRegistry::CreateRegistryKey(const String& mod, const String& key)
{
  return mod + "$" + key;
}

}