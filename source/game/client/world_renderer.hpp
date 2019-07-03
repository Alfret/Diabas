#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include "core/macros.hpp"
#include "game/world.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::graphics {
DIB_FORWARD_DECLARE_CLASS(Renderer);
DIB_FORWARD_DECLARE_CLASS(Camera);
}

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(GameClient);

// -------------------------------------------------------------------------- //

/** Render the terrain of the world **/
void
RenderWorldTerrain(graphics::Renderer& renderer,
                   const graphics::Camera& camera,
                   GameClient& gameClient);

// -------------------------------------------------------------------------- //

/** Pick a world position based on the mouse coordinates and the camera **/
WorldPos
PickWorldPosition(const World& world,
                  const graphics::Camera& camera,
                  Vector2F mousePos);

}