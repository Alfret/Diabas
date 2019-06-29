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

// -------------------------------------------------------------------------- //

void
ShowStatisticsDebug(GameClient& gameClient, f32 delta);

void
ShowScriptDebug(GameClient& gameClient);

// -------------------------------------------------------------------------- //

void
ShowModDebug(GameClient& gameClient);

// -------------------------------------------------------------------------- //

void
ShowTileDebug(GameClient& gameClient);

// -------------------------------------------------------------------------- //

void
ShowNetworkDebug(GameClient& gameClient);

}