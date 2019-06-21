#include "game.hpp"

#include <dlog.hpp>
#include "network/side.hpp"

// TEMP (for thread sleep to not overwork my linux machine)
#include <chrono>
#include <thread>

// ========================================================================== //
// Game Implementation
// ========================================================================== //

namespace dib {

Game::Game(const Descriptor& descriptor)
  : Application(descriptor)
  , mModLoader(Path{ "./mods" })
{
  GetGraphics().SetClearColor(graphics::Color{ 100, 149, 237 });

  DLOG_INFO("running as {}", SideToString(kSide));
  SetupInputCommands();

  mods::Result modResult = mModLoader.Load(mScriptEnvironment);
  DIB_ASSERT(modResult == mods::Result::kSuccess, "Failed to load mods");
  mModLoader.Init(world_);

  // script::Script testScript(mScriptEnvironment, Path{ "mods/core/main.js" });
  // script::Result result = testScript.Load();
  // DIB_ASSERT(result == script::Result::kSuccess, "Failed to load test
  // script");
}

// -------------------------------------------------------------------------- //

Game::~Game()
{
  // TODO do we need to explicitly call shutdown on Network here?
  std::exit(0);
}

// -------------------------------------------------------------------------- //

void
Game::Update(f64 delta)
{
  if (IsKeyDown(Key::kKeyEscape)) {
    Exit();
  }

  input_handler_.Update();

  mScriptEnvironment.Update();
  mModLoader.Update(delta);

  world_.Update();

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// -------------------------------------------------------------------------- //

void
Game::Render()
{}

// -------------------------------------------------------------------------- //

void
Game::OnKeyPressed(Key key)
{
  mModLoader.OnKeyPress(key);
}

// -------------------------------------------------------------------------- //

void
Game::OnKeyReleased(Key key)
{
  mModLoader.OnKeyRelease(key);
}

// -------------------------------------------------------------------------- //

void
Game::SetupInputCommands()
{
  input_handler_.AddCommand(
    InputCommandCategory::kInfo,
    "network",
    std::bind(&World::OnCommandNetwork, &world_, std::placeholders::_1));

  input_handler_.AddCommand(
    InputCommandCategory::kChat,
    "broadcast",
    std::bind(&World::OnCommandBroadcast, &world_, std::placeholders::_1));

  input_handler_.AddCommand(
    InputCommandCategory::kSystem, "exit", [](const std::string_view) {
      DLOG_INFO("Exiting the program");
      // TODO do we need to close the network before calling exit?
      std::exit(0);
    });

  input_handler_.AddCommand(
    InputCommandCategory::kSystem, "save", [](const std::string_view) {
      DLOG_INFO("Sorry to break it to you, but you cannot save yet..");
    });

  input_handler_.AddCommand(
    InputCommandCategory::kSystem, "filip", [](const std::string_view input) {
      DLOG_INFO("filip säger {}", input);
    });
}

}
