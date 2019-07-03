#include "game/gameplay/core_content.hpp"

// ========================================================================== //
// CoreContent Implementation
// ========================================================================== //

namespace dib::game {

CoreContent::CoreContent(ItemRegistry& itemRegistry, TileRegistry& tileRegistry)
{
  SetupItems(itemRegistry);
  SetupTiles(tileRegistry);
}

// -------------------------------------------------------------------------- //

CoreContent::~CoreContent()
{
  delete mTileRock;
  delete mTileDirt;
  delete mTileAir;
}

// -------------------------------------------------------------------------- //

void
CoreContent::GenerateWorld(World& world)
{
  for (u32 y = 0; y < world.GetTerrain().GetHeight(); y++) {
    for (u32 x = 0; x < world.GetTerrain().GetWidth(); x++) {
      if (y == 15) {
        world.GetTerrain().SetTile(WorldPos{ x, y }, mTileGrass);
      } else if (y < 15 && y > 5) {
        world.GetTerrain().SetTile(WorldPos{ x, y }, mTileDirt);
      } else if (y <= 5) {
        world.GetTerrain().SetTile(WorldPos{ x, y }, mTileRock);
      }
    }
  }
}

// -------------------------------------------------------------------------- //

void
CoreContent::SetupItems(ItemRegistry& itemRegistry)
{
  // Item: Apple
  mItemApple = new Item(
    ResourcePath{ Path{ "./res/items/apple.tga" } }, "apple", Vector2I(4, 1));
  itemRegistry.RegisterItem(MOD_ID, "apple", mItemApple);
}

// -------------------------------------------------------------------------- //

void
CoreContent::SetupTiles(TileRegistry& tileRegistry)
{
  // Tile: Air
  mTileAir = new Tile(ResourcePath{ Path{ "./res/tiles/air.tga" } }, "air");
  mTileAir->SetOpacity(0.0f)
    ->SetCollisionType(Tile::CollisionType::kNone)
    ->SetIsDestructible(false);
  tileRegistry.RegisterTile(MOD_ID, "air", mTileAir);

  // Tile: Dirt
  mTileDirt = new Tile(ResourcePath{ Path{ "./res/tiles/dirt.tga" } }, "dirt");
  tileRegistry.RegisterTile(MOD_ID, "dirt", mTileDirt);

  // Tile: Grass
  mTileGrass =
    new Tile(ResourcePath{ Path{ "./res/tiles/grass.tga" } }, "grass");
  tileRegistry.RegisterTile(MOD_ID, "grass", mTileGrass);

  // Tile: Rock
  mTileRock = new Tile(ResourcePath{ Path{ "./res/tiles/rock.tga" } }, "rock");
  tileRegistry.RegisterTile(MOD_ID, "rock", mTileRock);
}

}