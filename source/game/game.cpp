#include "game.hpp"

#include <dlog.hpp>
#include "network/network.hpp"

// TEMP (for thread sleep to not overwork my linux machine)
#include <chrono>
#include <thread>

// ========================================================================== //
// Game Implementation
// ========================================================================== //
#include <chrono>
#include <thread>
namespace dib {

Game::Game(const Descriptor& descriptor)
  : Application(descriptor)
  , mScriptEnvironment()
{
  GetGraphics().SetClearColor(graphics::Color{ 100, 149, 237 });

  DLOG_INFO("running as {}", SideToString(world_.GetSide()));
  SetupInputCommands();

  script::Script s(mScriptEnvironment, Path{ "mods/core/main.js" });
  script::Result result = s.Load();
  DIB_ASSERT(result == script::Result::kSuccess, "Failed to load test script");
}

// -------------------------------------------------------------------------- //

Game::~Game()
{
  Network::ShutdownNetwork();
  std::exit(0);
}

// -------------------------------------------------------------------------- //

void
Game::Update(f64 delta)
{
  if (IsKeyDown(Key::KEY_ESCAPE)) {
    Exit();
  }

  input_handler_.Update();

  mScriptEnvironment.Update();

  world_.Update();

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// -------------------------------------------------------------------------- //

void
Game::Render()
{}

// -------------------------------------------------------------------------- //

void
Game::SetupInputCommands()
{
  input_handler_.AddCommand(
    InputCommandCategory::kInfo,
    "network",
    std::bind(&World<kSide>::NetworkInfo, &world_, std::placeholders::_1));

  input_handler_.AddCommand(
    InputCommandCategory::kChat,
    "broadcast",
    std::bind(&World<kSide>::Broadcast, &world_, std::placeholders::_1));
}

}
