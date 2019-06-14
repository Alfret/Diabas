#include "game.hpp"
#include <dlog.hpp>
#include <functional>

// TEMP (for thread sleep to not overwork my linux machine)
#include <chrono>
#include <thread>

// ========================================================================== //
// Game Implementation
// ========================================================================== //
#include <chrono>
#include <thread>
namespace dib
{

Game::Game(const Descriptor& descriptor)
    : Application(descriptor)
{
  GetGraphics().SetClearColor(100 / 255.0f, 149 / 255.0f, 237 / 255.0f, 1.0f);
  DLOG_INFO("running as {}", SideToString(world.GetSide()));
}

// -------------------------------------------------------------------------- //

Game::~Game() {

}

// -------------------------------------------------------------------------- //

void
Game::Update(f64 delta)
{
  if (IsKeyDown(Key::KEY_ESCAPE)) {
    Exit();
  }

  world.Update();

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// -------------------------------------------------------------------------- //

void
Game::Render() {}


}
