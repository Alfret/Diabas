#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include "core/macros.hpp"
#include "game/world.hpp"
#include "game/terrain.hpp"

// ========================================================================== //
// Forward Declaration
// ========================================================================== //

namespace dib::graphics {

DIB_FORWARD_DECLARE_CLASS(Renderer);
DIB_FORWARD_DECLARE_CLASS(Camera);

}

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(ClientCache);

/** **/
class WorldRenderer : public Terrain::ChangeListener
{
public:
  struct Cell
  {
    /** Cached tile texture coordinates **/
    Vector2F texMinTile, texMaxTile;
    /** Cached wall texture coordinates **/
    Vector2F texMinWall, texMaxWall;
    /** Cached shadow information **/
  };

private:
  /** World to render **/
  World& mWorld;
  /** Client cache **/
  ClientCache& mClientCache;

  /** Cells **/
  Cell* mDataCells;

public:
  /** Construct world renderer **/
  WorldRenderer(World& world, ClientCache& clientCache);

  /** Destruct **/
  ~WorldRenderer();

  /** Render the world **/
  void Render(graphics::Renderer& renderer, const graphics::Camera& camera);

  /** Pick tile based on the camera position and the position of the mouse on
   * the window **/
  WorldPos PickScreenTile(const graphics::Camera& camera,
                          Vector2F mousePosition);

  void OnResize(u32 width, u32 height) override;

  void OnTileChanged(WorldPos pos) override;

  void OnWallChanged(WorldPos pos) override;

  /** Returns a cell in the world data **/
  Cell& GetCell(WorldPos pos);
};

}