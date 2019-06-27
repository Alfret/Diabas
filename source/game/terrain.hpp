#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include "game/tile/tile_manager.hpp"
#include "graphics/renderer.hpp"
#include "graphics/camera.hpp"

// ========================================================================== //
// Terrain Declaration
// ========================================================================== //

namespace dib::game {

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
  /** Tile manager **/
  TileManager& mTileManager;

  /** Width of terrain **/
  u32 mWidth;
  /** Height of terrain **/
  u32 mHeight;
  /** 2D array of tiles **/
  std::array<Tile::ID*, 3> mTerrainLayers;

public:
  Terrain(TileManager& tileManager, u32 width, u32 height);

  /** Draw the terrain **/
  void Draw(graphics::Renderer& renderer, graphics::Camera& camera);

  /** Returns the tile at the specified position in the terrain **/
  std::shared_ptr<Tile> GetTile(u32 x, u32 y, u32 layer);

  void SetTile(const std::shared_ptr<Tile>& tile, u32 x, u32 y, u32 layer);

  u32 GetWidth() const { return mWidth; };

  u32 GetHeight() const { return mHeight; };

private:
  Tile::ID* GetTerrainTileOffset(u32 x, u32 y, u32 layer);
};

}