#include "game/server/game_server.hpp"

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
