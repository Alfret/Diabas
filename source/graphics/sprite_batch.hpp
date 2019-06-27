#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <glad/glad.h>
#include "core/macros.hpp"
#include "core/types.hpp"
#include "graphics/texture.hpp"

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

  /** Currently bound texture **/
  std::shared_ptr<Texture> mCurrentTexture;

public:
  /** Construct **/
  SpriteBatch();

  /** Destruct **/
  ~SpriteBatch();

  /** Submit a sprite for rendering **/
  void Submit(const Sprite& sprite);

  /** Submit a textured rectangle **/
  void Submit(const std::shared_ptr<Texture>& texture,
              Vector3F position,
              Vector2F size);

private:
  /** Setup VBO and IBO **/
  void SetupBuffers();
};

}