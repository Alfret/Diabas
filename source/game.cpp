#include "game.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/assert.hpp"
#include "script/module.hpp"

// ========================================================================== //
// Game Implementation
// ========================================================================== //

namespace dib {

Game::Game(const Descriptor& descriptor)
  : Application(descriptor)
  , mScriptEnvironment()
{
  GetGraphics().SetClearColor(100 / 255.0f, 149 / 255.0f, 237 / 255.0f, 1.0f);

  script::Module* module =
    mScriptEnvironment.LoadModule(alflib::Path{ "mods/core/main.js" });
  DIB_ASSERT(module != nullptr, "Failed to load module");
  script::Result result = module->Run();
  int y = 0;
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
Game::Render()
{}

}
