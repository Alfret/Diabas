#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/macros.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::graphics {
DIB_FORWARD_DECLARE_CLASS(Renderer);
DIB_FORWARD_DECLARE_CLASS(Camera);
}

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(GameClient);

void
RenderWorldTerrain(graphics::Renderer& renderer,
                   const graphics::Camera& camera,
                   GameClient& gameClient);

}