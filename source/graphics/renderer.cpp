#include "graphics/renderer.hpp"

// ========================================================================== //
// Renderer Implementation
// ========================================================================== //

namespace dib::graphics {

Renderer::Renderer()
  : mClearColor(Color::CORNFLOWER_BLUE)
{}

// -------------------------------------------------------------------------- //

void
Renderer::NewFrame()
{
  // Clear buffers
  glClearColor(mClearColor.GetRedF32(),
               mClearColor.GetGreenF32(),
               mClearColor.GetBlueF32(),
               mClearColor.GetAlphaF32());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // New frame
  mSpriteBatch.NewFrame();
}

}