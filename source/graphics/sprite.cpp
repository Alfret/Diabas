#include "graphics/sprite.hpp"

// ========================================================================== //
// Sprite Implementation
// ========================================================================== //

namespace dib::graphics {

Sprite::Sprite()
  : mPosition({ 0.0f, 0.0f, 0.0f })
  , mScale({ 1.0f, 1.0f })
  , mAnchor({ 0.5f, 0.5f })
{}

// -------------------------------------------------------------------------- //

void
Sprite::SetPosition(const Vector3F& position)
{
  mPosition = position;
}

// -------------------------------------------------------------------------- //

void
Sprite::Move(const Vector3F& value)
{
  mPosition += value;
}

// -------------------------------------------------------------------------- //

void
Sprite::SetScale(const Vector2F& scale)
{
  mScale = scale;
}

// -------------------------------------------------------------------------- //

void
Sprite::SetAnchor(const Vector2F& anchor)
{
  mAnchor = anchor;
}

}
