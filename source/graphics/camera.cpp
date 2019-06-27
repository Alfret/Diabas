#include "graphics/camera.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <glm/gtc/matrix_transform.hpp>

// ========================================================================== //
// Camera Implementation
// ========================================================================== //

namespace dib::graphics {

Camera::Camera(u32 width, u32 height)
{
  mProjectionMatrix =
    glm::ortho(0.0f, f32(width), 0.0f, f32(height), -1.0f, 1.0f);
  glm::translate(mViewMatrix, Vector3F(0.0f, 0.0f, 0.0f));
  mMatrix = mProjectionMatrix * mViewMatrix;
}

// -------------------------------------------------------------------------- //

const Matrix4F&
Camera::GetViewProjectMatrix() const
{
  return mMatrix;
}

}