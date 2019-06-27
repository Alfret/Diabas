#include "graphics/sprite_batch.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "graphics/sprite.hpp"

// ========================================================================== //
// Shader Code
// ========================================================================== //

/** Source for the vertex shader used by the sprite batch **/
static const char8 sSpriteBatchVertexShaderSource[] = "";

/** Source for the fragment shader used by the sprite batch **/
static const char8 sSpriteBatchFragmentShaderSource[] = "";

// ========================================================================== //
// SpriteBatch Implementation
// ========================================================================== //

namespace dib::graphics {

SpriteBatch::SpriteBatch() {}

// -------------------------------------------------------------------------- //

SpriteBatch::~SpriteBatch() {}

// -------------------------------------------------------------------------- //

void
SpriteBatch::Submit(const Sprite& sprite)
{}

// -------------------------------------------------------------------------- //

void
SpriteBatch::Submit(const std::shared_ptr<Texture>& texture,
                    Vector3F position,
                    Vector2F size)
{}

// -------------------------------------------------------------------------- //

void
SpriteBatch::SetupBuffers()
{
  // Generate buffers
  glGenVertexArrays(1, &mVAO);
  glGenBuffers(1, &mVBO);
  glGenBuffers(1, &mIBO);

  // Setup vertex attributes
}

}