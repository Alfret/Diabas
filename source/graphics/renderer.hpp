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
  /** Clear color **/
  Color mClearColor;

  /** Sprite batch **/
  SpriteBatch mSpriteBatch;

public:
  /** Construct renderer **/
  Renderer();

  /** Begin a new frame **/
  void NewFrame();

  /** Returns the sprite batch **/
  SpriteBatch& GetSpriteBatch() { return mSpriteBatch; }

  /** Returns the number of draw calls made since new frame **/
  u32 GetDrawCallCount() { return mSpriteBatch.GetDrawCallCount(); }

  /** Returns the number of sprites drawn since new frame **/
  u32 GetDrawSpriteCount() const { return mSpriteBatch.GetDrawSpriteCount(); }

  /** Set the color to clear back buffers to on new frame **/
  void SetClearColor(Color color) { mClearColor = color; }
};

}