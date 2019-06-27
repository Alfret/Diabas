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
};

}