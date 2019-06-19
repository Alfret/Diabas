#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "graphics/sprite_batch.hpp"

// ========================================================================== //
// Renderer Declaration
// ========================================================================== //

namespace dib::graphics {

/** Renderer **/
class Renderer
{
private:
  /** Sprite batcher **/
  SpriteBatch mSpriteBatch;

public:
  Renderer();

  /** Returns the sprite batch **/
  SpriteBatch& GetSpriteBatch() { return mSpriteBatch; }
};

}