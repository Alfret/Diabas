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
  ConnectToServer();
}

// -------------------------------------------------------------------------- //

Game::~Game() {
  client_.CloseConnection();
}

// -------------------------------------------------------------------------- //

void
Game::Update(f64 delta)
{
  if (IsKeyDown(Key::KEY_ESCAPE)) {
    Exit();
  }

  UpdateNetwork();

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// -------------------------------------------------------------------------- //

void
Game::Render() {}

void
Game::UpdateNetwork() {
  static const auto callback_fn =
      std::bind(&Client::Poll, &client_);

  FixedTimeUpdate(kNetTicksPerSec, callback_fn);
}

void Game::ConnectToServer() {
  SteamNetworkingIPAddr addr{};
  addr.SetIPv4(0x7F000001, 24812);
  client_.Connect(addr);
}

}
