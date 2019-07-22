#include "game/client/game_client.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/constants.hpp"
#include <imgui/imgui.h>
#include "game/client/world_renderer.hpp"
#include "game/client/entity_render.hpp"
#include "game/client/debug_ui.hpp"
#include "game/gameplay/player.hpp"

// ========================================================================== //
// Client Implementation
// ========================================================================== //

#include <alflib/memory/memory_reader.hpp>
#include <alflib/memory/memory_writer.hpp>

namespace dib::game {

GameClient::GameClient(const app::AppClient::Descriptor& descriptor)
  : AppClient(descriptor)
  , mModLoader(Path{ "./mods" })
  , mCamera(GetWidth(), GetHeight())
  , mWorldRenderer(mWorld, mClientCache)
{
  CoreContent::Setup();

  mClientCache.BuildItemAtlas();
  mClientCache.BuildTileAtlas();
  mClientCache.BuildWallAtlas();

  CoreContent::GenerateWorld(mWorld);
}

// -------------------------------------------------------------------------- //

void
GameClient::Update(f64 delta)
{
  MICROPROFILE_SCOPEI("game client", "update", MP_CYAN);
  if (IsKeyDown(Key::kKeyEscape)) {
    Exit();
  }

  {
    MICROPROFILE_SCOPEI("game client", "ImGui", MP_CYAN1);

    // ImGui
    if (ImGui::Begin("Diabas - Debug")) {
      ShowStatisticsDebug(*this, delta);
      ShowScriptDebug(*this);
      ShowModDebug(*this);
      ShowItemDebug(*this);
      ShowTileDebug(*this);
      ShowWallDebug(*this);
      ShowWorldDebug(*this);
      ShowNetworkDebug(*this);
      ShowPlayerDebug(*this);
      ShowMisc(*this);
    }
    ImGui::End();
  }

  {
    MICROPROFILE_SCOPEI("game client", "game", MP_CYAN2);
    UpdateCamera(delta);
    Player::Update(*this, delta);
    mWorld.Update(delta);
  }
}

// -------------------------------------------------------------------------- //

void
GameClient::OnNetworkTick()
{
  MICROPROFILE_SCOPEI("game client", "network tick", MP_ORANGE2);

  auto& network = mWorld.GetNetwork();
  if (auto maybe_entity = network.GetOurPlayerEntity(); maybe_entity) {
    auto& registry = mWorld.GetEntityManager().GetRegistry();
    auto& packet = network.GetReusablePacket();
    packet.SetHeader(network.GetPacketHandler(), PacketHeaderStaticTypes::kTick);
    auto mw = packet.GetMemoryWriter();

    { // 1. Player data
      mw->Write(registry.get<game::Moveable>(*maybe_entity).ToIncrement());
    }

    mw.Finalize();
    network.PacketBroadcast(packet);
  }
}

// -------------------------------------------------------------------------- //

void
GameClient::Render()
{
  mRenderer.NewFrame();

  mWorldRenderer.Render(mRenderer, mCamera);
  RenderEntities(mRenderer, mCamera, mWorld.GetEntityManager());
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
