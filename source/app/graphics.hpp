#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/types.hpp"
#include "graphics/color.hpp"

// ========================================================================== //
// Graphics Declaration
// ========================================================================== //

namespace dib {

/** Graphics interface **/
class Graphics
{
private:
  /** Reference to window **/
  GLFWwindow* mWindowRef;

  /** Device name **/
  String mDeviceName;

public:
  /** Create grahics interface from descriptor **/
  explicit Graphics(GLFWwindow* window);

  /** Destroy graphics interface **/
  ~Graphics();

  /** Returns the name of the graphics device **/
  const String& DeviceName() const { return mDeviceName; }

  /** Resize buffers **/
  void Resize(u32 width, u32 height);

  /** Clear render target **/
  void Clear();

  /** Present to the window **/
  void Present();

  /** Set the clear color **/
  void SetClearColor(const graphics::Color& color);

private:
  /** OpenGL debug callback function **/
  static void DebugCallbackGL(GLenum source,
                              GLenum type,
                              GLuint id,
                              GLenum severity,
                              GLsizei length,
                              const GLchar* message,
                              void* userParam);
};

}