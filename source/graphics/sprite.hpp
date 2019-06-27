#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include "graphics/texture.hpp"

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

  /** Texture for sprite **/
  std::shared_ptr<Texture> mTexture;
  /** Min texture coordinates **/
  Vector2F mTexMin;
  /** Max texture coordinates **/
  Vector2F mTexMax;

public:
  /** Construct sprite at origin **/
  explicit Sprite(std::shared_ptr<Texture> texture,
                  Vector2F texMin = Vector2F(0.0f, 0.0f),
                  Vector2F texMax = Vector2F(1.0f, 1.0f));

  /** Set position of the sprite **/
  void SetPosition(const Vector3F& position);

  /** Move sprite relative to current position **/
  void Move(const Vector3F& value);

  /** Set scale **/
  void SetScale(const Vector2F& scale);

  /** Set the anchor (center-point) around which the sprite rotates **/
  void SetAnchor(const Vector2F& anchor);

  /** Returns the texture **/
  const std::shared_ptr<Texture>& GetTexture() const { return mTexture; }

  /** Get min texture coordinates **/
  const Vector2F& GetTexMin() const { return mTexMin; }

  /** Get max texture coordinates **/
  const Vector2F& GetTexMax() const { return mTexMax; }
};

}
