#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include "core/macros.hpp"
#include "graphics/camera.hpp"
#include "game/tile/tile.hpp"

// ========================================================================== //
// Terrain Declaration
// ========================================================================== //

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(TileManager);
DIB_FORWARD_DECLARE_CLASS(World);

/** 2D terrain **/
class Terrain
{
public:
  /** Foreground layer **/
  static constexpr u32 LAYER_FOREGROUND = 0;
  /** Background layer **/
  static constexpr u32 LAYER_BACKGROUND = 1;
  /** Wiring layer **/
  static constexpr u32 LAYER_WIRING = 2;

private:
  /** World **/
  World& mWorld;
  /** Tile manager **/
  TileManager& mTileManager;

  /** Width of terrain **/
  u32 mWidth;
  /** Height of terrain **/
  u32 mHeight;
  /** 2D array of tiles **/
  std::array<Tile::ID*, 3> mTerrainLayers;

public:
  Terrain(World& world, TileManager& tileManager, u32 width, u32 height);

  void Generate();

  /** Returns the tile at the specified position in the terrain **/
  std::shared_ptr<Tile> GetTile(u32 x, u32 y, u32 layer);

  void SetTile(const std::shared_ptr<Tile>& tile, u32 x, u32 y, u32 layer);

  /** Pick a tile **/
  void PickTile(const graphics::Camera& camera,
                f64 mouseX,
                f64 mouseY,
                u32& tileX,
                u32& tileY);

  u32 GetWidth() const { return mWidth; };

  u32 GetHeight() const { return mHeight; };

private:
  Tile::ID* GetTerrainTileOffset(u32 x, u32 y, u32 layer);
};

}