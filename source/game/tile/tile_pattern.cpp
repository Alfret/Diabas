#include "game/tile/tile_pattern.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/world.hpp"
#include "game/terrain.hpp"

// ========================================================================== //
// TilePatternRug Implementation
// ========================================================================== //

namespace dib::game {

TilePatternRug::TilePatternRug(ResourcePath resourcePath)
  : Tile(resourcePath)
{
  AlfAssert(resourcePath.GetType() == ResourceType::kTextureTileRug,
            "'TilePatternRug' expects the resource path to point to a "
            "rug-pattern atlas for tiles");

  // Add sub-resource paths
  for (u32 i = 0; i < RUG_PATTERN_SUB_RESOURCE_COUNT; i++) {
    const Path& resPath = resourcePath.GetResourcePath();
    String pathName =
      resPath.GetDirectory().Join(resPath.GetBaseName()).GetPathString();
    pathName += "_" + String::ToString(i) + resPath.GetExtensionString();

    if (resourcePath.GetModId() != "builtin") {
      mSubPaths.emplace_back(
        ResourceType::kTexture, resourcePath.GetModId(), Path{ pathName });
    } else {
      mSubPaths.emplace_back(ResourceType::kTexture, Path{ pathName });
    }
  }
}

// -------------------------------------------------------------------------- //

ResourcePath&
TilePatternRug::GetResourcePath(World& world, u32 x, u32 y, u32 layer)
{
  // Retrieve neighbours
  std::shared_ptr<Tile> up =
    world.GetTerrain().GetTile(x, y + 1, Terrain::LAYER_FOREGROUND);
  std::shared_ptr<Tile> down =
    world.GetTerrain().GetTile(x, y - 1, Terrain::LAYER_FOREGROUND);
  std::shared_ptr<Tile> left =
    world.GetTerrain().GetTile(x - 1, y, Terrain::LAYER_FOREGROUND);
  std::shared_ptr<Tile> right =
    world.GetTerrain().GetTile(x + 1, y, Terrain::LAYER_FOREGROUND);

  // Determine texture
  u32 row = 3;
  if (up->GetID() == mID) {
    row = down->GetID() == mID ? 1 : 2;
  } else if (down->GetID() == mID) {
    row = 0;
  }
  u32 column = 0;
  if (left->GetID() == mID) {
    column = right->GetID() ? 2 : 1;
  } else if (right->GetID() == mID) {
    column = 3;
  }

  return mSubPaths[row * 4 + column];
}

}

// ========================================================================== //
// TilePatternBlob Implementation
// ========================================================================== //

namespace dib::game {}