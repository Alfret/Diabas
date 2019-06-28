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
  : mWidth(width)
  , mHeight(height)
{
  mProjectionMatrix =
    glm::ortho(0.0f, f32(width), 0.0f, f32(height), -1.0f, 1.0f);
  mViewMatrix = Matrix4F(1.0f);
  glm::translate(mViewMatrix, Vector3F(0.0f, 0.0f, 0.0f));
  mMatrix = mProjectionMatrix * mViewMatrix;
}

// -------------------------------------------------------------------------- //

const Matrix4F&
Camera::GetViewProjectMatrix() const
{
  return mMatrix;
}

// -------------------------------------------------------------------------- //

void
Camera::Resize(u32 width, u32 height)
{
  mWidth = width;
  mHeight = height;
  mProjectionMatrix =
    glm::ortho(0.0f, f32(width), 0.0f, f32(height), -1.0f, 1.0f);
  mViewMatrix = Matrix4F(1.0f);
  glm::translate(mViewMatrix, Vector3F(0.0f, 0.0f, 0.0f));
  mMatrix = mProjectionMatrix * mViewMatrix;
}

// -------------------------------------------------------------------------- //

void
Camera::Move(const Vector3F& distance)
{
  mPosition += distance;
  mViewMatrix = Matrix4F(1.0f);
  glm::translate(mViewMatrix, mPosition);
  mMatrix = mProjectionMatrix * mViewMatrix;
}

// -------------------------------------------------------------------------- //

void
Camera::ClampPosition(const Vector3F& min, const Vector3F& max)
{
  mPosition.x = alflib::Clamp(mPosition.x, min.x, max.x);
  mPosition.y = alflib::Clamp(mPosition.y, min.y, max.y);
  mPosition.z = alflib::Clamp(mPosition.z, min.z, max.z);
}

}