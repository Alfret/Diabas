#include "game/gameplay/core_content.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <microprofile/microprofile.h>

// ========================================================================== //
// CoreContent Implementation
// ========================================================================== //

namespace dib::game {

CoreContent::~CoreContent()
{
  delete mTileChair;
  delete mTileRock;
  delete mTileGrass;
  delete mTileDirt;
  delete mTileAir;
}

// -------------------------------------------------------------------------- //

CoreContent&
CoreContent::GetInstance()
{
  static CoreContent instance;
  return instance;
}

// -------------------------------------------------------------------------- //

void
CoreContent::Setup(ItemRegistry& itemRegistry, TileRegistry& tileRegistry)
{
  GetInstance().SetupItems(itemRegistry);
  GetInstance().SetupTiles(tileRegistry);
}

// -------------------------------------------------------------------------- //

void
CoreContent::GenerateWorld(World& world)
{
  CoreContent& instance = GetInstance();

  MICROPROFILE_SCOPEI("CoreGame", "WorldGen", MP_BROWN2);

  for (u32 y = 0; y < world.GetTerrain().GetHeight(); y++) {
    for (u32 x = 0; x < world.GetTerrain().GetWidth(); x++) {
      if (y == 15) {
        world.GetTerrain().GenSetTile(WorldPos{ x, y }, instance.mTileGrass);
      } else if (y < 15 && y > 5) {
        world.GetTerrain().GenSetTile(WorldPos{ x, y }, instance.mTileDirt);
      } else if (y <= 5) {
        world.GetTerrain().GenSetTile(WorldPos{ x, y }, instance.mTileRock);
      } else {
        world.GetTerrain().GenSetTile(WorldPos{ x, y }, instance.mTileAir);
      }
    }
  }
  world.GetTerrain().ReCacheResourceIndices();
}

// -------------------------------------------------------------------------- //

void
CoreContent::SetupItems(ItemRegistry& itemRegistry)
{
  MICROPROFILE_SCOPEI("CoreGame", "SetupItems", MP_PINK);

  // Item: Apple
  mItemApple = new Item(
    ResourcePath{ Path{ "./res/items/apple.tga" } }, "apple", Vector2I(4, 1));
  itemRegistry.RegisterItem(MOD_ID, "apple", mItemApple);
}

// -------------------------------------------------------------------------- //

void
CoreContent::SetupTiles(TileRegistry& tileRegistry)
{
  MICROPROFILE_SCOPEI("CoreGame", "SetupTile", MP_BISQUE3);

  // Tile: Air
  mTileAir = new Tile(ResourcePath{ Path{ "./res/tiles/air.tga" } }, "air");
  mTileAir->SetOpacity(0.0f)
    ->SetCollisionType(CollisionType::kNone)
    ->SetIsDestructible(false)
    ->SetCanBeReplaced(true);
  tileRegistry.RegisterTile(MOD_ID, "air", mTileAir);

  // Tile: Dirt
  mTileDirt = new Tile(ResourcePath{ Path{ "./res/tiles/dirt.tga" } }, "dirt");
  tileRegistry.RegisterTile(MOD_ID, "dirt", mTileDirt);

  // Tile: Grass
  mTileGrass =
    new Tile(ResourcePath{ Path{ "./res/tiles/grass.tga" } }, "grass");
  tileRegistry.RegisterTile(MOD_ID, "grass", mTileGrass);

  // Tile: Chair
  mTileChair =
    new TileChair(ResourcePath{ Path{ "./res/tiles/chair.tga" } }, "chair");
  tileRegistry.RegisterTile(MOD_ID, "chair", mTileChair);

  // Tile: Rock
  mTileRock = new Tile(ResourcePath{ Path{ "./res/tiles/rock.tga" } }, "rock");
  tileRegistry.RegisterTile(MOD_ID, "rock", mTileRock);

  // Tile: Spikes
  mTileSpikes = new Tile(ResourcePath{ Path{ "./res/tiles/spikes.tga" } }, "spikes");
  tileRegistry.RegisterTile(MOD_ID, "spikes", mTileSpikes);
}

}
