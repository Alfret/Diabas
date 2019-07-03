#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/item/item_registry.hpp"
#include "game/tile/tile_registry.hpp"
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

  /** Apple item **/
  Item* mItemApple = nullptr;

public:
  /** Construct core content **/
  CoreContent(ItemRegistry& itemRegistry, TileRegistry& tileRegistry);

  /** Destruct core content **/
  ~CoreContent();

  /** Generate the base world **/
  void GenerateWorld(World& world);

private:
  /** Setup core items **/
  void SetupItems(ItemRegistry& itemRegistry);

  /** Setup core tiles **/
  void SetupTiles(TileRegistry& tileRegistry);
};

}