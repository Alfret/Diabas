#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"

// ========================================================================== //
// Sprite Declaration
// ========================================================================== //

namespace dib::graphics {

/** Sprite **/
class Sprite
{
private:
  /** Sprite position **/
  Vector3F mPosition;
  /** Sprite scale **/
  Vector2F mScale;
  /** Sprite anchor (center) **/
  Vector2F mAnchor;

public:
  /** Construct sprite at origin **/
  Sprite();

  /** Set position of the sprite **/
  void SetPosition(const Vector3F& position);

  /** Move sprite relative to current position **/
  void Move(const Vector3F& value);

  /** Set scale **/
  void SetScale(const Vector2F& scale);

  /** Set the anchor (center-point) around which the sprite rotates **/
  void SetAnchor(const Vector2F& anchor);
};

}
