#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "app/client/app_client.hpp"
#include "game/world.hpp"
#include "game/client/client_cache.hpp"
#include "game/client/world_renderer.hpp"
#include "game/gameplay/core_content.hpp"
#include "game/mod/mod_loader.hpp"
#include "graphics/camera.hpp"
#include "graphics/renderer.hpp"

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

  /** Mod loader **/
  game::ModLoader mModLoader;

  /** Renderer **/
  graphics::Renderer mRenderer;
  /** Camera **/
  graphics::Camera mCamera;

  /** Client resource cache **/
  ClientCache mClientCache;

  /** World renderer **/
  WorldRenderer mWorldRenderer;

public:
  /** Construct game client **/
  explicit GameClient(const AppClient::Descriptor& descriptor);

  /** Update client **/
  void Update(f64 delta) override;

  /// Send our client state to server
  void OnNetworkTick() final;

  /** Render client **/
  void Render() override;

  void OnWindowResize(u32 width, u32 height) override;

  /** Returns the world **/
  World& GetWorld() { return mWorld; }

  /** Returns the world **/
  const World& GetWorld() const { return mWorld; }

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

  /** Returns the world renderer **/
  WorldRenderer& GetWorldRenderer() { return mWorldRenderer; }

  /** Returns the world renderer **/
  const WorldRenderer& GetWorldRenderer() const { return mWorldRenderer; }

private:
  /** Update the game camera **/
  void UpdateCamera(f32 delta);
};

}
