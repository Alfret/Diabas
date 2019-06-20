#include "app/graphics.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/assert.hpp"

// ========================================================================== //
// Graphics Implementation
// ========================================================================== //

namespace dib {

Graphics::Graphics(GLFWwindow* window)
  : mWindowRef(window)
{
  // Make context current and initialize Glad
  glfwMakeContextCurrent(mWindowRef);
  const int result = gladLoadGL();
  DIB_ASSERT(result != 0, "Failed to initialize GLAD");

  // Retrieve name of device
  const GLubyte* renderer = glGetString(GL_RENDERER);
  mDeviceName = String(reinterpret_cast<const char*>(renderer));
}

// -------------------------------------------------------------------------- //

Graphics::~Graphics() {}

// -------------------------------------------------------------------------- //

void
Graphics::Resize(u32 width, u32 height)
{
  glViewport(0, 0, width, height);
}

// -------------------------------------------------------------------------- //

void
Graphics::Clear()
{
  glClear(GL_COLOR_BUFFER_BIT);
}

// -------------------------------------------------------------------------- //

void
Graphics::Present()
{
  glfwSwapBuffers(mWindowRef);
}

// -------------------------------------------------------------------------- //

void
Graphics::SetClearColor(const graphics::Color& color)
{
  glClearColor(color.Red(), color.Green(), color.Blue(), color.Alpha());
}

}
