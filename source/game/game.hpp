#pragma once

#include "mods/loader.hpp"
#include "app/app.hpp"
#include "game/world.hpp"
#include "network/side.hpp"
#include "server/input.hpp"
#include "script/env.hpp"
#include "graphics/renderer.hpp"
#include "audio/audio_manager.hpp"
#include "game/tile/tile_manager.hpp"
#include "game/terrain.hpp"

// ========================================================================== //
// Game Declaration
// ========================================================================== //

namespace dib {

/** Game class**/
class Game : public Application
{

  // ============================================================ //
  // Lifetime
  // ============================================================ //
public:
  explicit Game(const Descriptor& descriptor);

  ~Game() override;

  // ============================================================ //
  // Methods
  // ============================================================ //
public:
  void Update(f64 delta) override;

  void Render() override;

  void OnKeyPressed(Key key) override;

  void OnKeyReleased(Key key) override;
  void OnWindowResize(u32 width, u32 height) override;

  graphics::Camera& GetCamera() { return mCamera; }

  const graphics::Camera& GetCamera() const { return mCamera; }

  game::Terrain& GetTerrain() { return mTerrain; }

  const game::Terrain& GetTerrain() const { return mTerrain; }

  // ============================================================ //
  // Private Methods
  // ============================================================ //
private:
  void SetupInputCommands();

  // ============================================================ //
  // Member Variables
  // ============================================================ //
private:
  /** Script environment **/
  script::Environment mScriptEnvironment;
  /** Renderer **/
  graphics::Renderer mRenderer;
  /** Camera **/
  graphics::Camera mCamera;

  /** Mod loader **/
  game::ModLoader mModLoader;

  /** Tile manager **/
  game::TileManager mTileManager;
  /** Item manager **/

  /** Terrain **/
  game::Terrain mTerrain;

  World world_{};

  InputHandler<kSide> input_handler_{};

  AudioManager audio_manager_{};
};

}
