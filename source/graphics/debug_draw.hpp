#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "graphics/vertex_buffer.hpp"
#include "graphics/index_buffer.hpp"
#include "graphics/shader.hpp"
#include "graphics/camera.hpp"

// ========================================================================== //
// DebugDraw Declaration
// ========================================================================== //

namespace dib::graphics {

/** Debug drawing **/
class DebugDraw
{
public:
  /** Max number of lines that can be batched together **/
  static constexpr u32 MAX_LINES = 50000;
  /** Max number of vertices **/
  static constexpr u32 MAX_VERTICES = MAX_LINES * 2;

  /** Index of position attribute **/
  static constexpr u32 VERTEX_ATTRIBUTE_POS = 0;
  /** Index of color attribute **/
  static constexpr u32 VERTEX_ATTRIBUTE_COL = 1;

private:
  /** Vertex structure **/
  struct Vertex
  {
    /** Position **/
    Vector2F position;
    /** Tint **/
    alflib::Color color = { 0, 0, 0, 1 };
  };

public:
  /** Debug line **/
  struct Line
  {
    Vector2F from;
    Vector2F to;
    Color color;
  };

  /** Debug rectangle **/
  struct Rect
  {
    Vector2F position;
    Vector2F size;
    Color color;
  };

  /** Debug Circle **/
  struct Circle
  {
    Vector2F position;
    f32 radius;
    Color color;
  };

private:
  /** Vertex array **/
  GLuint mVAO;
  /** Vertex buffer **/
  VertexBuffer mVertexBuffer;

  /** Shader program **/
  ShaderProgram* mShaderProgram;

  /** CPU data buffer (uploaded to VBO on each render) **/
  std::vector<Vertex> mLineData;
  /** Number of sprites that are represented in data **/
  u32 mLineCount;

  /** Current camera **/
  const Camera* mCamera = nullptr;

  /** Number of draw calls done this frame **/
  u32 mDrawCallCount = 0;
  /** Number of lines drawn this frame **/
  u32 mDrawLineCount = 0;

public:
  /** Construct **/
  DebugDraw();

  /** Destruct **/
  ~DebugDraw();

  /** Called by the renderer to signal new frame **/
  void NewFrame();

  void Begin(const Camera* camera);

  void End();

  /** Flush batch **/
  void Flush();

  /** Submit a line for rendering **/
  void Submit(const Line& line);

  /** Submit a line for rendering **/
  void Submit(const Vector2F& from, const Vector2F& to, Color color);

  /** Submit a rect for rendering **/
  void Submit(const Rect& rect);

  /** Submit circle for rendering **/
  void Submit(const Circle& circle);

  /** Returns the number of draw calls made since new frame **/
  [[nodiscard]] u32 GetDrawCallCount() const { return mDrawCallCount; }

  /** Returns the number of sprites drawn since new frame **/
  [[nodiscard]] u32 GetDrawLineCount() const { return mDrawLineCount; }

private:
  /** Setup VBO and IBO **/
  void SetupBuffers();

  /** Setup shader program **/
  void SetupShaders();
};

}