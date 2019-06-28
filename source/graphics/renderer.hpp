#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <alflib/graphics/color.hpp>

#include "graphics/sprite_batch.hpp"
#include "graphics/texture.hpp"
#include "game/terrain.hpp"

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

  /** Clear the color buffer to a color **/
  void Clear(alflib::Color color);

  /** Returns the sprite batch **/
  SpriteBatch& GetSpriteBatch() { return mSpriteBatch; }
};

}