#include "app/ogl/graphics_ogl.hpp"

#include "glad/glad.h"
#include "core/assert.hpp"

// ========================================================================== //
// GraphicsOpenGL Implementation
// ========================================================================== //

namespace dib {

GraphicsOpenGL::GraphicsOpenGL(const Graphics::Descriptor& descriptor)
{
  // Create context
  mWindowRef = descriptor.window;
  mHandle = SDL_GL_CreateContext(descriptor.window);
  SDL_GL_MakeCurrent(mWindowRef, mHandle);
  int result = gladLoadGL();
  DIB_ASSERT(result != 0, "Failed to initialize GLAD");

  // Retrieve name of device
  const GLubyte* renderer = glGetString(GL_RENDERER);
  mDeviceName = String(reinterpret_cast<const char*>(renderer));
}

// -------------------------------------------------------------------------- //

GraphicsOpenGL::~GraphicsOpenGL() 
{
  SDL_GL_DeleteContext(mHandle);
}

// -------------------------------------------------------------------------- //

void
GraphicsOpenGL::SetClearColor(f32 r, f32 g, f32 b, f32 a)
{
  glClearColor(r, g, b, a);
}

// -------------------------------------------------------------------------- //

void
GraphicsOpenGL::Clear()
{
  glClear(GL_COLOR_BUFFER_BIT);
}

// -------------------------------------------------------------------------- //

void
GraphicsOpenGL::Present()
{
  SDL_GL_SwapWindow(mWindowRef);
}

}
