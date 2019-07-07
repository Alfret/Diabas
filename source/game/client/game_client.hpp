#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "app/client/app_client.hpp"
#include "game/world.hpp"
#include "game/mods/loader.hpp"
#include "game/client/client_cache.hpp"
#include "game/gameplay/core_content.hpp"
#include "graphics/camera.hpp"
#include "graphics/renderer.hpp"
#include "script/env.hpp"
#include "game/client/player.hpp"

// ========================================================================== //
// Client Declaration
// ========================================================================== //

namespace dib::game {

/** Game client **/
class GameClient : public app::AppClient
{
private:
  /** Item registry **/
  ItemRegistry mItemRegistry;
  /** Tile registry **/
  TileRegistry mTileRegistry;
  /** Game world **/
  World mWorld;

  /** Content **/
  CoreContent mCoreContent;

  /** Script environment **/
  script::Environment mScriptEnvironment;
  /** Mod loader **/
  game::ModLoader mModLoader;

  /** Renderer **/
  graphics::Renderer mRenderer;
  /** Camera **/
  graphics::Camera mCamera;

  /** Client resource cache **/
  ClientCache mClientCache;

  Player mPlayer{};

public:
  /** Construct game client **/
  explicit GameClient(const AppClient::Descriptor& descriptor);

  /** Update client **/
  void Update(f64 delta) override;

  /** Render client **/
  void Render() override;

  void OnWindowResize(u32 width, u32 height) override;

  /** Returns the item registry **/
  ItemRegistry& GetItemRegistry() { return mItemRegistry; }

  /** Returns the item registry **/
  const ItemRegistry& GetItemRegistry() const { return mItemRegistry; }

  /** Returns the tile registry **/
  TileRegistry& GetTileRegistry() { return mTileRegistry; }

  /** Returns the tile registry **/
  const TileRegistry& GetTileRegistry() const { return mTileRegistry; }

  /** Returns the world **/
  World& GetWorld() { return mWorld; }

  /** Returns the world **/
  const World& GetWorld() const { return mWorld; }

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

  /** Returns the renderer **/
  graphics::Renderer& GetRenderer() { return mRenderer; }

  /** Returns the renderer **/
  const graphics::Renderer& GetRenderer() const { return mRenderer; }

  /** Returns the camera **/
  graphics::Camera& GetCamera() { return mCamera; }

  /** Returns the camera **/
  const graphics::Camera& GetCamera() const { return mCamera; }

  /** Returns the client cache **/
  ClientCache& GetCache() { return mClientCache; }

  /** Returns the client cache **/
  const ClientCache& GetCache() const { return mClientCache; }

private:
  /** Update the game camera **/
  void UpdateCamera(f32 delta);
};

}
