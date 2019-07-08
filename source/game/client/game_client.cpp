#include "game/client/game_client.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/constants.hpp"
#include <imgui/imgui.h>
#include "game/client/world_renderer.hpp"
#include "game/client/debug_ui.hpp"

// ========================================================================== //
// Client Implementation
// ========================================================================== //

namespace dib::game {

GameClient::GameClient(const app::AppClient::Descriptor& descriptor)
  : AppClient(descriptor)
  , mWorld(mTileRegistry)
  , mModLoader(Path{ "./mods" })
  , mCamera(GetWidth(), GetHeight())
{
  CoreContent::Setup(mItemRegistry, mTileRegistry);

  mClientCache.BuildTileAtlas(mTileRegistry);
  mClientCache.BuildItemAtlas(mItemRegistry);

  CoreContent::GenerateWorld(mWorld);
}

// -------------------------------------------------------------------------- //

void
GameClient::Update(f64 delta)
{
  if (IsKeyDown(Key::kKeyEscape)) {
    Exit();
  }

  ImGui::ShowTestWindow();

  // ImGui
  if (ImGui::Begin("Diabas - Debug")) {
    ShowStatisticsDebug(*this, delta);
    ShowScriptDebug(*this);
    ShowModDebug(*this);
    ShowTileDebug(*this);
    ShowItemDebug(*this);
    ShowNetworkDebug(*this);
    ShowPlayerDebug(*this);
  }
  ImGui::End();

  UpdateCamera(delta);
  mPlayer.Update(*this, delta);
  mWorld.Update();
}

// -------------------------------------------------------------------------- //

void
GameClient::Render()
{
  mRenderer.NewFrame();

  RenderWorldTerrain(mRenderer, mCamera, *this);
}

// -------------------------------------------------------------------------- //

void
GameClient::OnWindowResize(u32 width, u32 height)
{
  mCamera.Resize(width, height);
}

// -------------------------------------------------------------------------- //

void
GameClient::UpdateCamera(f32 delta)
{
  // Move camera
  Vector3F cameraMove(0.0f, 0.0f, 0.0f);
  f32 cameraSpeed = 300;
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
  Vector3F maxPos = Vector3F(
    mWorld.GetTerrain().GetWidth() * game::TILE_SIZE - mCamera.GetWidth(),
    mWorld.GetTerrain().GetHeight() * game::TILE_SIZE - mCamera.GetHeight(),
    1.0f);
  mCamera.ClampPosition(minPos, maxPos);
}

}
