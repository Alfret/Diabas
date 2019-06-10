#include "game.hpp"
#include "script/script.hpp"

// ========================================================================== //
// Game Implementation
// ========================================================================== //

namespace dib
{

Game::Game(const Descriptor& descriptor)
  : Application(descriptor)
{
  GetGraphics().SetClearColor(100 / 255.0f, 149 / 255.0f, 237 / 255.0f, 1.0f);


  Script s0("res/demo.js");


}

// -------------------------------------------------------------------------- //

Game::~Game() {}

// -------------------------------------------------------------------------- //

void
Game::Update(f64 delta)
{
  if (IsKeyDown(Key::KEY_ESCAPE)) {
    Exit();
  }
}

// -------------------------------------------------------------------------- //

void
Game::Render() {}

}
