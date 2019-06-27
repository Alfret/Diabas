#include "game/tile/tile.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

// ========================================================================== //
// Tile Implementation
// ========================================================================== //

namespace dib::game {

Tile::Tile(const ResourcePath& resourcePath)
{
  mResourcePaths.push_back(resourcePath);
}

// -------------------------------------------------------------------------- //

ResourcePath&
Tile::GetResourcePath([[maybe_unused]] World& world,
                      [[maybe_unused]] u32 x,
                      [[maybe_unused]] u32 y)
{
  return mResourcePaths[0];
}

// -------------------------------------------------------------------------- //

std::vector<ResourcePath>&
Tile::GetResourcePaths()
{
  return mResourcePaths;
}

}