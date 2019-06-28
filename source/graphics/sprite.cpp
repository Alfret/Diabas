#include "graphics/sprite.hpp"

// ========================================================================== //
// Sprite Implementation
// ========================================================================== //

namespace dib::graphics {

Sprite::Sprite(std::shared_ptr<Texture> texture,
               Vector2F size,
               Vector2F texMin,
               Vector2F texMax)
  : mPosition({ 0.0f, 0.0f, 0.0f })
  , mSize(size)
  , mAnchor({ 0.5f, 0.5f })
  , mTexture(std::move(texture))
  , mTexMin(texMin)
  , mTexMax(texMax)
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
Sprite::SetSize(const Vector2F& size)
{
  mSize = size;
}

// -------------------------------------------------------------------------- //

void
Sprite::SetAnchor(const Vector2F& anchor)
{
  mAnchor = anchor;
}

}
