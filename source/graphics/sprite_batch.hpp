#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <glad/glad.h>
#include <alflib/graphics/color.hpp>
#include "core/macros.hpp"
#include "core/types.hpp"
#include "graphics/texture.hpp"
#include "graphics/shader.hpp"
#include "graphics/vertex_buffer.hpp"
#include "graphics/index_buffer.hpp"
#include "graphics/camera.hpp"

// ========================================================================== //
// SpriteBatch Declaration
// ========================================================================== //

namespace dib::graphics {

DIB_FORWARD_DECLARE_CLASS(Sprite);

/** Class that is used for batching submissions of sprites together so that they
 * all can be rendered together **/
class SpriteBatch
{
public:
  static constexpr u32 MAX_SPRITES = 100000;
  static constexpr u32 MAX_VERTICES = MAX_SPRITES * 4;
  static constexpr u32 MAX_INDICES = MAX_SPRITES * 6;

  static constexpr u32 VERTEX_ATTRIBUTE_POS = 0;
  static constexpr u32 VERTEX_ATTRIBUTE_TINT = 1;
  static constexpr u32 VERTEX_ATTRIBUTE_UV = 2;

private:
  /** Vertex structure **/
  struct Vertex
  {
    /** Position **/
    Vector3F position;
    /** Tint **/
    alflib::Color tint = { 0, 0, 0, 1 };
    /** Texture coordinates **/
    Vector2F uv;
  };

private:
  /** Vertex array **/
  GLuint mVAO;
  /** Vertex buffer **/
  VertexBuffer mVertexBuffer;
  /** Index buffer **/
  IndexBuffer mIndexBuffer;

  /** Shader program **/
  ShaderProgram* mShaderProgram;

  /** CPU data buffer (uploaded to VBO on each render) **/
  std::vector<Vertex> mData;
  /** Number of sprites that are represented in data **/
  u32 mDataCount;

  /** Currently bound texture **/
  std::shared_ptr<Texture> mCurrentTexture;
  /** Current camera **/
  const Camera* mCamera = nullptr;

  /** Number of draw calls done this frame **/
  u32 mDrawCallCount = 0;
  /** Number of sprites drawn this frame **/
  u32 mDrawSpriteCount = 0;

public:
  /** Construct **/
  SpriteBatch();

  /** Destruct **/
  ~SpriteBatch();

  /** Called by the renderer to signal new frame **/
  void NewFrame();

  void Begin(const Camera* camera);

  void End();

  /** Flush batch **/
  void Flush();

  /** Submit a sprite for rendering **/
  void Submit(const Sprite& sprite);

  /** Submit a textured rectangle **/
  void Submit(const std::shared_ptr<Texture>& texture,
              Vector3F position,
              Vector2F size = Vector2F(16.0f, 16.0f),
              alflib::Color color = alflib::Color::WHITE,
              Vector2F texMin = Vector2F(0.0f, 0.0f),
              Vector2F texMax = Vector2F(1.0f, 1.0f));

  /** Returns the number of draw calls made since new frame **/
  u32 GetDrawCallCount() const { return mDrawCallCount; }

  /** Returns the number of sprites drawn since new frame **/
  u32 GetDrawSpriteCount() const { return mDrawSpriteCount; }

private:
  /** Setup VBO and IBO **/
  void SetupBuffers();

  /** Setup shader program **/
  void SetupShaders();
};

}