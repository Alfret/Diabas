#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/item/item_registry.hpp"
#include "game/tile/tile_registry.hpp"
#include "game/wall/wall_registry.hpp"
#include "game/gameplay/tile/tile_chair.hpp"
#include "game/gameplay/tile/tile_variant.hpp"
#include "game/world.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::game {

/** Class representing the core of the gameplay. This contains all the tiles
 * and items that are part of the base game **/
class CoreContent
{
public:
  /** Name of the core content "mod" **/
  static constexpr char8 MOD_ID[] = "core";

private:
  /** Items **/
  struct Items
  {
    /** Apple  **/
    Item* apple = nullptr;
  } mItems;

  /** Tiles **/
  struct Tiles
  {
    /** Air **/
    Tile* air = nullptr;
    /** Dirt **/
    Tile* dirt = nullptr;
    /** Grass **/
    Tile* grass = nullptr;
    /** Rock **/
    Tile* rock = nullptr;

    Tile* mTileSpikes = nullptr;

    /** Dungeon wall **/
    TileVariant* dungeon = nullptr;

    /** Chair **/
    TileChair* chair = nullptr;
  } mTiles;

  /** Walls **/
  struct Walls
  {
    /** Air **/
    Wall* air = nullptr;
    /** Stone **/
    Wall* stone = nullptr;
  } mWalls;

public:
  /** Destruct core content **/
  ~CoreContent();

public:
  /** Returns the singleton instance **/
  static CoreContent& GetInstance();

  /** Setup items and tiles **/
  static void Setup();

  /** Generate the base world **/
  static void GenerateWorld(World& world);

  /** Returns core items **/
  static Items& GetItems() { return GetInstance().mItems; }

  /** Returns core tiles **/
  static Tiles& GetTiles() { return GetInstance().mTiles; }

  /** Returns core walls **/
  static Walls& GetWalls() { return GetInstance().mWalls; }

private:
  /** Construct core content **/
  CoreContent() = default;

  /** Setup core items **/
  void SetupItems();

  /** Setup core tiles **/
  void SetupTiles();

  /** Setup core walls **/
  void SetupWalls();
};

}
