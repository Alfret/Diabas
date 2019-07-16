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
  // Delete items

  // Delete tiles
  delete mTiles.chair;
  delete mTiles.rock;
  delete mTiles.grass;
  delete mTiles.dirt;
  delete mTiles.air;

  // Delete walls
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
  GetInstance().SetupWalls();
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
        world.GetTerrain().GenSetTile(WorldPos{ x, y }, instance.mTiles.grass);
        // world.GetTerrain().GenSetWall(WorldPos{ x, y },
        // instance.mWalls.stone);
      } else if (y < 15 && y > 5) {
        world.GetTerrain().GenSetTile(WorldPos{ x, y }, instance.mTiles.dirt);
        // world.GetTerrain().GenSetWall(WorldPos{ x, y },
        // instance.mWalls.stone);
      } else if (y <= 5) {
        world.GetTerrain().GenSetTile(WorldPos{ x, y }, instance.mTiles.rock);
        // world.GetTerrain().GenSetWall(WorldPos{ x, y },
        // instance.mWalls.stone);
      } else {
        world.GetTerrain().GenSetTile(WorldPos{ x, y }, instance.mTiles.air);
        // world.GetTerrain().GenSetWall(WorldPos{ x, y }, instance.mWalls.air);
      }

      world.GetTerrain().GenSetWall(WorldPos{ x, y }, instance.mWalls.air);
    }
  }

  world.GetTerrain().GenSetWall(WorldPos{ 25, 25 }, instance.mWalls.stone);

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
  mItems.apple = new Item(
    ResourcePath{ Path{ "./res/items/apple.tga" } }, "apple", Vector2I(4, 1));
  itemRegistry.RegisterItem(MOD_ID, "apple", mItems.apple);
}

// -------------------------------------------------------------------------- //

void
CoreContent::SetupTiles()
{
  MICROPROFILE_SCOPEI("CoreGame", "SetupTile", MP_BISQUE3);

  TileRegistry& tileRegistry = TileRegistry::Instance();

  // Tile: Air
  mTiles.air = new Tile(ResourcePath{ Path{ "./res/tiles/air.tga" } }, "air");
  mTiles.air->SetOpacity(0.0f)
    ->SetCollisionType(CollisionType::kNone)
    ->SetIsDestructible(false)
    ->SetCanBeReplaced(true);
  tileRegistry.RegisterTile(MOD_ID, "air", mTiles.air);

  // Tile: Dirt
  mTiles.dirt =
    new Tile(ResourcePath{ Path{ "./res/tiles/dirt.tga" } }, "dirt");
  tileRegistry.RegisterTile(MOD_ID, "dirt", mTiles.dirt);

  // Tile: Grass
  mTiles.grass =
    new Tile(ResourcePath{ Path{ "./res/tiles/grass.tga" } }, "grass");
  tileRegistry.RegisterTile(MOD_ID, "grass", mTiles.grass);

  // Tile: Chair
  mTiles.chair =
    new TileChair(ResourcePath{ Path{ "./res/tiles/chair.tga" } }, "chair");
  tileRegistry.RegisterTile(MOD_ID, "chair", mTiles.chair);

  // Tile: Rock
  mTiles.rock =
    new Tile(ResourcePath{ Path{ "./res/tiles/rock.tga" } }, "rock");
  tileRegistry.RegisterTile(MOD_ID, "rock", mTiles.rock);

  // Tile: Spikes
  mTiles.spikes =
    new TileSpikes(ResourcePath{ Path{ "./res/tiles/spikes.tga" } }, "spikes");
  tileRegistry.RegisterTile(MOD_ID, "spikes", mTiles.spikes);

  // Tile: Dungeon wall
  mTiles.dungeon = new TileVariant(
    ResourcePath{ Path{ "./res/tiles/dungeon_wall.png" } }, "dungeon_wall");
  tileRegistry.RegisterTile(MOD_ID, "dungeon_wall", mTiles.dungeon);
}

// -------------------------------------------------------------------------- //

void
CoreContent::SetupWalls()
{
  WallRegistry& wallRegistry = WallRegistry::Instance();

  // Air
  mWalls.air = new Wall(ResourcePath{ Path{ "./res/walls/wall_air.png" },
                                      ResourcePath::Flag::kBorder1px },
                        "air_wall");
  mWalls.air->SetCanBeReplaced(true);
  wallRegistry.RegisterWall(MOD_ID, "air_wall", mWalls.air);

  // Stone
  mWalls.stone = new Wall(ResourcePath{ Path{ "./res/walls/wall_stone.png" },
                                        ResourcePath::Flag::kBorder1px },
                          "stone_wall");
  wallRegistry.RegisterWall(MOD_ID, "stone_wall", mWalls.stone);
}

}
