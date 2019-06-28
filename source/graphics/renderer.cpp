#include "graphics/renderer.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/tile/tile_manager.hpp"

// ========================================================================== //
// Renderer Implementation
// ========================================================================== //

namespace dib::graphics {

Renderer::Renderer() {}

// -------------------------------------------------------------------------- //

void
Renderer::Clear(alflib::Color color)
{
  glClearColor(color.GetRedF32(),
               color.GetGreenF32(),
               color.GetBlueF32(),
               color.GetAlphaF32());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

}