#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <alflib/graphics/color.hpp>

#include "graphics/debug_draw.hpp"
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

  /** Debug draw **/
  DebugDraw mDebugDraw;

public:
  /** Construct renderer **/
  Renderer();

  /** Begin a new frame **/
  void NewFrame();

  /** Returns the sprite batch **/
  SpriteBatch& GetSpriteBatch() { return mSpriteBatch; }

  /** Returns the debug draw **/
  DebugDraw& GetDebugDraw() { return mDebugDraw; }

  /** Returns the number of draw calls made since new frame **/
  u32 GetDrawCallCount()
  {
    return mSpriteBatch.GetDrawCallCount() + mDebugDraw.GetDrawCallCount();
  }

  /** Returns the number of sprites drawn since new frame **/
  [[nodiscard]] u32 GetDrawSpriteCount() const
  {
    return mSpriteBatch.GetDrawSpriteCount() + mDebugDraw.GetDrawLineCount();
  }

  /** Set the color to clear back buffers to on new frame **/
  void SetClearColor(Color color) { mClearColor = color; }
};

}