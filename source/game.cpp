#include "game.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/assert.hpp"
#include "script/script.hpp"
#include "script/util.hpp"

// ========================================================================== //
// Game Implementation
// ========================================================================== //

namespace dib {

Game::Game(const Descriptor& descriptor)
  : Application(descriptor)
  , mScriptEnvironment()
{
  GetGraphics().SetClearColor(graphics::Color{ 100, 149, 237 });

  script::Script s(mScriptEnvironment, Path{ "mods/core/main.js" });
  script::Result result = s.Load();
  DIB_ASSERT(result == script::Result::kSuccess, "Failed to load test script");
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
