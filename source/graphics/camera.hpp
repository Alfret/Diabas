#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"

// ========================================================================== //
// Camera Declaration
// ========================================================================== //

namespace dib::graphics {

/** Camera **/
class Camera
{
private:
  /** Width of viewport **/
  u32 mWidth;
  /** Height of viewport **/
  u32 mHeight;

  /** Position **/
  Vector3F mPosition = Vector3F(0.0f, 0.0f, 0.0f);

  /** View matrix **/
  Matrix4F mViewMatrix;
  /** Projection matrix **/
  Matrix4F mProjectionMatrix;

  /** Concatenated matrix **/
  Matrix4F mMatrix;

public:
  /** Construct camera from width and height of the viewport (window) **/
  Camera(u32 width, u32 height);

  /** Returns the concatenated view and projection matrix **/
  const Matrix4F& GetViewProjectMatrix() const;

  /** Resize camera viewport **/
  void Resize(u32 width, u32 height);

  /** Move camera **/
  void Move(const Vector3F& distance);

  /** Clamp the camera position **/
  void ClampPosition(const Vector3F& min, const Vector3F& max);

  /** Returns the viewport width **/
  [[nodiscard]] u32 GetWidth() const { return mWidth; }

  /** Returns the viewport height **/
  [[nodiscard]] u32 GetHeight() const { return mHeight; }

  /** Returns the position **/
  const Vector3F& GetPosition() const { return mPosition; }
};

}