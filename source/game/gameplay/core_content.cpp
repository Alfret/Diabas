#include "game/gameplay/core_content.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <microprofile/microprofile.h>
#include <dutil/stopwatch.hpp>

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
CoreContent::Setup()
{
  GetInstance().SetupItems();
  GetInstance().SetupTiles();
}

// -------------------------------------------------------------------------- //

void
CoreContent::GenerateWorld(World& world)
{
  CoreContent& instance = GetInstance();

  dutil::Stopwatch sw;
  sw.Start();

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

#if 0
  for (u32 i = 0; i < 256; i++) {
    u32 x = 20 + (i % 16) * 4;
    u32 y = 20 + (i / 16) * 4;

    WorldPos p{ x, y };
    world.GetTerrain().GenSetTile(p, instance.mTileDungeon);

    if (i & Bit(0)) {
      world.GetTerrain().GenSetTile(p.TopLeft(), instance.mTileDungeon);
    }
    if (i & Bit(1)) {
      world.GetTerrain().GenSetTile(p.Top(), instance.mTileDungeon);
    }
    if (i & Bit(2)) {
      world.GetTerrain().GenSetTile(p.TopRight(), instance.mTileDungeon);
    }
    if (i & Bit(3)) {
      world.GetTerrain().GenSetTile(p.Left(), instance.mTileDungeon);
    }
    if (i & Bit(4)) {
      world.GetTerrain().GenSetTile(p.Right(), instance.mTileDungeon);
    }
    if (i & Bit(5)) {
      world.GetTerrain().GenSetTile(p.BottomLeft(), instance.mTileDungeon);
    }
    if (i & Bit(6)) {
      world.GetTerrain().GenSetTile(p.Bottom(), instance.mTileDungeon);
    }
    if (i & Bit(7)) {
      world.GetTerrain().GenSetTile(p.BottomRight(), instance.mTileDungeon);
    }
  }
#endif

  world.GetTerrain().ReCacheResourceIndices();

  sw.Stop();
  f64 seconds = sw.fs();
  DLOG_VERBOSE("World generation (Core) finished in {:.3f}s", seconds);
}

// -------------------------------------------------------------------------- //

void
CoreContent::SetupItems()
{
  MICROPROFILE_SCOPEI("CoreGame", "SetupItems", MP_PINK);

  ItemRegistry& itemRegistry = ItemRegistry::Instance();

  // Item: Apple
  mItemApple = new Item(
    ResourcePath{ Path{ "./res/items/apple.tga" } }, "apple", Vector2I(4, 1));
  itemRegistry.RegisterItem(MOD_ID, "apple", mItemApple);
}

// -------------------------------------------------------------------------- //

void
CoreContent::SetupTiles()
{
  MICROPROFILE_SCOPEI("CoreGame", "SetupTile", MP_BISQUE3);

  TileRegistry& tileRegistry = TileRegistry::Instance();

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

  // Tile: Dungeon wall
  mTileDungeon = new TileVariant(
    ResourcePath{ Path{ "./res/tiles/dungeon_wall.png" } }, "dungeon_wall");
  tileRegistry.RegisterTile(MOD_ID, "dungeon_wall", mTileDungeon);
}

}
