#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <glad/glad.h>
#include "core/macros.hpp"
#include "core/types.hpp"

// ========================================================================== //
// SpriteBatch Declaration
// ========================================================================== //

namespace dib::graphics {

DIB_FORWARD_DECLARE_CLASS(Sprite);

/** Class that is used for batching submissions of sprites together so that they
 * all can be rendered together **/
class SpriteBatch
{
private:
  /** Vertex structure **/
  struct Vertex
  {
    /** Position **/
    Vector3F position;
    /** Texture coordinates **/
    Vector2F uv;
  };

private:
  /** Vertex array **/
  GLuint mVAO;
  /** Vertex buffer **/
  GLuint mVBO;
  /** Index buffer **/
  GLuint mIBO;

  /** CPU data buffer (uploaded to VBO on each render) **/

public:
  /** Construct **/
  SpriteBatch();

  /** Destruct **/
  ~SpriteBatch();

  /** Submit a sprite for rendering **/
  void Submit(const Sprite& sprite);

private:
  /** Setup VBO and IBO **/
  void SetupBuffers();
};

}