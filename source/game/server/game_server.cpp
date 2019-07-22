#include "game/server/game_server.hpp"
#include <microprofile/microprofile.h>

// ========================================================================== //
// Client Implementation
// ========================================================================== //

namespace dib::game {

GameServer::GameServer(const AppServer::Descriptor& descriptor)
  : AppServer(descriptor)
  , mWorld()
  , mModLoader(Path{ "./mods" })
{
  CoreContent::Setup();

  RegisterCommands();

  CoreContent::GenerateWorld(mWorld);
}

// -------------------------------------------------------------------------- //

void
GameServer::Update(f64 delta)
{
  mCLI.Update();
  mWorld.Update(delta);
}

// -------------------------------------------------------------------------- //

void
GameServer::OnNetworkTick()
{
  MICROPROFILE_SCOPEI("game server", "network tick", MP_ORANGE2);

  auto& registry = mWorld.GetEntityManager().GetRegistry();
  auto& network = mWorld.GetNetwork();
  auto& packet = network.GetReusablePacket();
  packet.SetHeader(network.GetPacketHandler(), PacketHeaderStaticTypes::kTick);
  auto mw = packet.GetMemoryWriter();

  { // 1. Players
    auto view = registry.view<PlayerData, Moveable, Soul>();
    AlfAssert(mw->Write(static_cast<u32>(view.size())), "write failed");

    for (const auto entity : view) {
      AlfAssert(mw->Write(view.get<PlayerData>(entity).uuid), "write failed");
      AlfAssert(mw->Write(view.get<Moveable>(entity).ToIncrement()),
                "write failed");
      AlfAssert(mw->Write(view.get<Soul>(entity)), "write failed");
    }
  }

  { // 2. Npc's
    auto& em = mWorld.GetEntityManager();
    auto& npc_registry = mWorld.GetNpcRegistry();
    AlfAssert(mw->Write(static_cast<u32>(npc_registry.GetNpcs().size())),
              "write failed");
    for (const auto npc : npc_registry.GetNpcs()) {
      AlfAssert(mw->Write(npc.second->GetID()), "write failed");
      npc.second->ToIncrement(em, *(*mw));
    }
  }

  mw.Finalize();
  network.PacketBroadcast(packet);
}

// -------------------------------------------------------------------------- //

void
GameServer::RegisterCommands()
{
  // Command: Network info
  mCLI.AddCommand(
    InputCommandCategory::kInfo,
    "network",
    std::bind(&World::OnCommandNetwork, &mWorld, std::placeholders::_1));

  // Command: Broadcast chat
  mCLI.AddCommand(
    InputCommandCategory::kChat,
    "broadcast",
    std::bind(&World::OnCommandBroadcast, &mWorld, std::placeholders::_1));

  // Command: Exit
  mCLI.AddCommand(InputCommandCategory::kSystem,
                  "exit",
                  std::bind(
                    [](const std::string_view view, GameServer* server) {
                      if (!view.empty()) {
                        DLOG_INFO("Shutting down server ({})", view);
                      } else {
                        DLOG_INFO("Shutting down server");
                      }
                      server->Exit();
                    },
                    std::placeholders::_1,
                    this));

  // Command: Save world
  mCLI.AddCommand(
    InputCommandCategory::kSystem, "save", [](const std::string_view) {
      DLOG_INFO("Sorry to break it to you, but you cannot save yet..");
    });

  mCLI.AddCommand(
    InputCommandCategory::kInfo, "packet_types", [&](const std::string_view) {
      mWorld.GetNetwork().GetPacketHandler().PrintPacketTypes();
    });
}

}
