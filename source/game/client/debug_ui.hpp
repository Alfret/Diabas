#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include "core/macros.hpp"

// ========================================================================== //
// DebugUI Declaration
// ========================================================================== //

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(GameClient);

/** Debugging UI **/
class DebugUI
{
private:
  /** Client that the UI is displayed for **/
  GameClient& mGameClient;

public:
  /** Construct debug UI for client **/
  DebugUI(GameClient& gameClient);

  /** Update UI **/
  void Update(f32 delta);

private:
  void ShowScriptDebug();

  void ShowModDebug();

  void ShowTileDebug();
};

}