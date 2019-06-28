#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "app/client/app_client.hpp"
#include "game/world.hpp"
#include "game/mods/loader.hpp"
#include "game/tile/tile_manager.hpp"
#include "game/client/tile_atlas.hpp"
#include "graphics/camera.hpp"
#include "graphics/renderer.hpp"
#include "script/env.hpp"

// ========================================================================== //
// Client Declaration
// ========================================================================== //

namespace dib::game {

/** Game client **/
class GameClient : public app::AppClient
{
private:
  /** Game world **/
  World mWorld;

  /** Tile atlas **/
  TileAtlas mTileAtlas;

  /** Script environment **/
  script::Environment mScriptEnvironment;
  /** Mod loader **/
  game::ModLoader mModLoader;

  /** Renderer **/
  graphics::Renderer mRenderer;
  /** Camera **/
  graphics::Camera mCamera;

public:
  /** Construct game client **/
  explicit GameClient(const AppClient::Descriptor& descriptor);

  /** Update client **/
  void Update(f64 delta) override;

  /** Render client **/
  void Render() override;

  /** Returns the world **/
  World& GetWorld() { return mWorld; }

  /** Returns the world **/
  const World& GetWorld() const { return mWorld; }

  /** Returns the tile atlas **/
  TileAtlas& GetTileAtlas() { return mTileAtlas; }

  /** Returns the tile atlas **/
  const TileAtlas& GetTileAtlas() const { return mTileAtlas; }

  /** Returns the script environment **/
  script::Environment& GetScriptEnvironment() { return mScriptEnvironment; }

  /** Returns the script environment **/
  const script::Environment& GetScriptEnvironment() const
  {
    return mScriptEnvironment;
  }

  /** Returns the mod loader **/
  ModLoader& GetModLoader() { return mModLoader; }

  /** Returns the mod loader **/
  const ModLoader& GetModLoader() const { return mModLoader; }

  /** Returns the camera **/
  graphics::Camera& GetCamera() { return mCamera; }

  /** Returns the camera **/
  const graphics::Camera& GetCamera() const { return mCamera; }

private:
  /** Update the game camera **/
  void UpdateCamera(f32 delta);
};

}