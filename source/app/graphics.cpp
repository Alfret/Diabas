#include "app/graphics.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>
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

  // Setup debug
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback((GLDEBUGPROC)Graphics::DebugCallbackGL, this);
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
Graphics::SetClearColor(alflib::Color color)
{
  glClearColor(color.GetRedF32(),
               color.GetGreenF32(),
               color.GetBlueF32(),
               color.GetAlphaF32());
}

// -------------------------------------------------------------------------- //

void
Graphics::DebugCallbackGL(GLenum source,
                          GLenum type,
                          GLuint id,
                          GLenum severity,
                          GLsizei length,
                          const GLchar* message,
                          void* userParam)
{
  auto graphics = static_cast<Graphics*>(userParam);
  if (type == GL_DEBUG_TYPE_ERROR) {
    DLOG_ERROR("OpenGL: {}", message);
  } else if (type == GL_DEBUG_TYPE_PERFORMANCE ||
             type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR ||
             type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) {
    DLOG_WARNING("OpenGL: {}", message);
  } else {
    // DLOG_VERBOSE("OpenGL: {}", message);
  }
}

}
