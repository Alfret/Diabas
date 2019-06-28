#include "game/client/game_client.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/client/world_renderer.hpp"

// ========================================================================== //
// Client Implementation
// ========================================================================== //

namespace dib::game {

GameClient::GameClient(const app::AppClient::Descriptor& descriptor)
  : AppClient(descriptor)
  , mModLoader(mScriptEnvironment, Path{ "./mods" })
  , mCamera(GetWidth(), GetHeight())
  , mDebugUI(*this)
{
  mModLoader.Init(mWorld);
  mModLoader.RegisterTiles(mWorld.GetTileManager());
  mTileAtlas.Build(mWorld.GetTileManager());
  mWorld.GetTerrain().Generate();
}

// -------------------------------------------------------------------------- //

void
GameClient::Update(f64 delta)
{
  UpdateCamera(delta);
  mDebugUI.Update(delta);
}

// -------------------------------------------------------------------------- //

void
GameClient::Render()
{
  mRenderer.Clear(alflib::Color::CORNFLOWER_BLUE);

  WorldRenderer::RenderWorld(mRenderer, mTileAtlas, mWorld, mCamera);
}

// -------------------------------------------------------------------------- //

void
GameClient::UpdateCamera(f32 delta)
{
  // Move camera
  Vector3F cameraMove(0.0f, 0.0f, 0.0f);
  f32 cameraSpeed = 80;
  if (IsKeyDown(Key::kKeyLeft)) {
    cameraMove.x -= cameraSpeed * delta;
  }
  if (IsKeyDown(Key::kKeyRight)) {
    cameraMove.x += cameraSpeed * delta;
  }
  if (IsKeyDown(Key::kKeyUp)) {
    cameraMove.y += cameraSpeed * delta;
  }
  if (IsKeyDown(Key::kKeyDown)) {
    cameraMove.y -= cameraSpeed * delta;
  }
  mCamera.Move(cameraMove);

  Vector3F minPos = Vector3F(0.0f, 0.0f, 0.0f);
  Vector3F maxPos =
    Vector3F(mWorld.GetTerrain().GetWidth() * game::TileManager::TILE_SIZE -
               mCamera.GetWidth(),
             mWorld.GetTerrain().GetHeight() * game::TileManager::TILE_SIZE -
               mCamera.GetHeight(),
             1.0f);
  mCamera.ClampPosition(minPos, maxPos);
}

}