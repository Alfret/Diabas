#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/item/item_registry.hpp"
#include "game/tile/tile_registry.hpp"
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
  /** Air tile **/
  Tile* mTileAir = nullptr;
  /** Dirt tile **/
  Tile* mTileDirt = nullptr;
  /** Grass tile **/
  Tile* mTileGrass = nullptr;
  /** Rock tile **/
  Tile* mTileRock = nullptr;

  /** Tile: Dungeon wall **/
  TileVariant* mTileDungeon = nullptr;

  /** Chair tile **/
  TileChair* mTileChair = nullptr;

  /** Apple item **/
  Item* mItemApple = nullptr;

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

  /** Returns the air tile **/
  static Tile* GetTileAir() { return GetInstance().mTileAir; }

  /** Returns the chair tile **/
  static TileChair* GetTileChair() { return GetInstance().mTileChair; }

private:
  /** Construct core content **/
  CoreContent() = default;

  /** Setup core items **/
  void SetupItems();

  /** Setup core tiles **/
  void SetupTiles();
};

}