#include "game/wall/wall_registry.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/assert.hpp"
#include "game/constants.hpp"

// ========================================================================== //
// WallRegistry Implementation
// ========================================================================== //

namespace dib::game {

WallRegistry::WallRegistry()
{
  // Register invalid tile
  mBuiltin.wallInvalid =
    new Wall(ResourcePath{ Path{ "./res/tiles/invalid.tga" } }, "invalid");
  RegisterWall(CreateRegistryKey(BUILTIN_MOD_NAME, "invalid"),
               mBuiltin.wallInvalid);

  // Register missing tile
  mBuiltin.wallMissing =
    new Wall(ResourcePath{ Path{ "./res/tiles/missing.tga" } }, "missing");
  RegisterWall(CreateRegistryKey(BUILTIN_MOD_NAME, "missing"),
               mBuiltin.wallMissing);
}

// -------------------------------------------------------------------------- //

WallRegistry::~WallRegistry()
{
  delete mBuiltin.wallMissing;
  delete mBuiltin.wallInvalid;
}

// -------------------------------------------------------------------------- //

bool
WallRegistry::RegisterWall(const String& modId,
                           const String& registryKey,
                           Wall* wall)
{
  return RegisterWall(CreateRegistryKey(modId, registryKey), wall);
}

// -------------------------------------------------------------------------- //

bool
WallRegistry::RegisterWall(const String& registryKey, Wall* wall)
{
  DIB_ASSERT(wall != nullptr, "Cannot register null wall");

  // Make sure it's not already registered
  if (mWallRegistryMap.count(registryKey) > 0) {
    return false;
  }

  // Register wall with next ID
  mWalls.push_back(wall);
  const WallID id = mWalls.size() - 1;

  // Setup maps
  mWallRegistryMap[registryKey] = id;
  mWallMap[wall] = id;
  return true;
}

// -------------------------------------------------------------------------- //

Wall*
WallRegistry::GetWall(WallRegistry::WallID id) const
{
  return mWalls[id];
}

// -------------------------------------------------------------------------- //

Wall*
WallRegistry::GetWall(const String& registryKey) const
{
  return GetWall(GetWallID(registryKey));
}

// -------------------------------------------------------------------------- //

WallRegistry::WallID
WallRegistry::GetWallID(const String& registryKey) const
{
  return mWallRegistryMap.at(registryKey);
}

// -------------------------------------------------------------------------- //

WallRegistry::WallID
WallRegistry::GetWallID(Wall* wall) const
{
  return mWallMap.at(wall);
}

// -------------------------------------------------------------------------- //

WallRegistry&
WallRegistry::Instance()
{
  static WallRegistry instance;
  return instance;
}

// -------------------------------------------------------------------------- //

String
WallRegistry::CreateRegistryKey(const String& mod, const String& key)
{
  return mod + "$" + key;
}

}