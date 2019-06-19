#include "game.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/assert.hpp"
#include "script/util.hpp"

// ========================================================================== //
// Game Implementation
// ========================================================================== //

namespace dib {

Game::Game(const Descriptor& descriptor)
  : Application(descriptor)
  , mScriptEnvironment()
{
  GetGraphics().SetClearColor(100 / 255.0f, 149 / 255.0f, 237 / 255.0f, 1.0f);

  mScriptEnvironment.LoadModule(alflib::Path{ "mods/core/main.js" });
}

// -------------------------------------------------------------------------- //

Game::~Game() {}

// -------------------------------------------------------------------------- //

void
Game::Update(f64 delta)
{
  // Quit game if the escape key is pressed
  if (IsKeyDown(Key::KEY_ESCAPE)) {
    Exit();
  }

  // Update the script manager
  mScriptEnvironment.Update();
}

// -------------------------------------------------------------------------- //

void
Game::Render()
{}

}
