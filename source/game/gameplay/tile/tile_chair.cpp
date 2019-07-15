#include "game/gameplay/tile/tile_chair.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/world.hpp"
#include "game/gameplay/core_content.hpp"

// ========================================================================== //
// TileChair Implementation
// ========================================================================== //

namespace dib::game {

TileChair::TileChair(const ResourcePath& resourcePath,
                     const String& translationKey)
  : Tile(resourcePath, translationKey)
{}

// -------------------------------------------------------------------------- //

bool
TileChair::IsMultiTile([[maybe_unused]] World& world,
                       [[maybe_unused]] WorldPos pos)
{
  return true;
}

// -------------------------------------------------------------------------- //

bool
TileChair::CanPlaceMultiTile(World& world, WorldPos pos)
{
  Terrain& terrain = world.GetTerrain();
  return terrain.IsValidPosition(pos.Top()) &&
         terrain.GetTile(pos.Top()) == CoreContent::GetTiles().air;
}

// -------------------------------------------------------------------------- //

bool
TileChair::PlaceMultiTile(World& world, WorldPos pos)
{
  Terrain& terrain = world.GetTerrain();
  if (terrain.IsValidPosition(pos.Top())) {
    Tile* topTile = terrain.GetTile(pos.Top());
    if (topTile != CoreContent::GetTiles().air) {
      return false;
    }

    terrain.GenSetTile(pos.Top(), CoreContent::GetTiles().chair, true);
    terrain.SetMetadata(pos.Top(), 0);
    terrain.SetMetadata(pos, 2);
    return true;
  }
  return false;
}

// -------------------------------------------------------------------------- //

bool
TileChair::KillMultiTile(World& world, WorldPos pos)
{
  Terrain& terrain = world.GetTerrain();
  if (terrain.GetMetadata(pos) == 0) {
    terrain.GenSetTile(pos, CoreContent::GetTiles().air, true);
    terrain.GenSetTile(pos.Bottom(), CoreContent::GetTiles().air, true);
  } else if (terrain.GetMetadata(pos) == 2) {
    terrain.GenSetTile(pos.Top(), CoreContent::GetTiles().air, true);
    terrain.GenSetTile(pos, CoreContent::GetTiles().air, true);
  }
  return true;
}

// -------------------------------------------------------------------------- //

u32
TileChair::GetResourceIndex(World& world, WorldPos pos)
{
  return world.GetTerrain().GetMetadata(pos);
}

}