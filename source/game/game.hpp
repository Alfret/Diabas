#pragma once

#include "mods/loader.hpp"
#include "app/app.hpp"
#include "game/world.hpp"
#include "network/side.hpp"
#include "server/input.hpp"
#include "script/env.hpp"
#include "graphics/renderer.hpp"
#include "audio/audio_manager.hpp"

// ========================================================================== //
// Game Declaration
// ========================================================================== //

namespace dib {

/** Game class**/
class Game : public Application
{

  // ============================================================ //
  // Lifetime
  // ============================================================ //
public:
  explicit Game(const Descriptor& descriptor);

  ~Game() override;

  // ============================================================ //
  // Methods
  // ============================================================ //
public:
  void Update(f64 delta) override;

  void Render() override;

  void OnKeyPressed(Key key) override;

  void OnKeyReleased(Key key) override;

  // ============================================================ //
  // Private Methods
  // ============================================================ //
private:
  void SetupInputCommands();

  // ============================================================ //
  // Member Variables
  // ============================================================ //
private:
  /** Script environment **/
  script::Environment mScriptEnvironment;
  /** Renderer **/
  graphics::Renderer mRenderer;

  /** Mod loader **/
  mods::ModLoader mModLoader;

  World world_{};

  InputHandler<kSide> input_handler_{};

  AudioManager audio_manager_{};
};

}
