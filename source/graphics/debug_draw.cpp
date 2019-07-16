#include "graphics/debug_draw.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "math/math.hpp"

// ========================================================================== //
// DebugDraw Implementation
// ========================================================================== //

namespace dib::graphics {

DebugDraw::DebugDraw()
  : mVertexBuffer(8, nullptr, true, "DebugDrawVB")
{
  SetupBuffers();
  SetupShaders();
}

// -------------------------------------------------------------------------- //

DebugDraw::~DebugDraw() {}

// -------------------------------------------------------------------------- //

void
DebugDraw::NewFrame()
{
  mDrawCallCount = 0;
  mDrawLineCount = 0;
}

// -------------------------------------------------------------------------- //

void
DebugDraw::Begin(const Camera* camera)
{
  mCamera = camera;
  mLineData.resize(0);
  mLineCount = 0;
}

// -------------------------------------------------------------------------- //

void
DebugDraw::End()
{
  // Don't render if there are nothing to render
  if (mLineCount < 1) {
    return;
  }

  AlfAssert(mCamera != nullptr, "'Begin' must have been called before 'End'");

  mVertexBuffer.Upload(reinterpret_cast<const u8*>(mLineData.data()),
                       mLineData.size() * sizeof(Vertex));
  mShaderProgram->Bind();
  mShaderProgram->SetUniformMatrix4("u_view_proj",
                                    mCamera->GetViewProjectMatrix());
  glBindVertexArray(mVAO);

  glDisable(GL_DEPTH_TEST);
  glLineWidth(1.0f);
  glDrawArrays(GL_LINES, 0, mLineCount * 2);

  mDrawCallCount++;
  mDrawLineCount += mLineCount;
}

// -------------------------------------------------------------------------- //

void
DebugDraw::Flush()
{
  End();
  Begin(mCamera);
}

// -------------------------------------------------------------------------- //

void
DebugDraw::Submit(const DebugDraw::Line& line)
{
  Submit(line.from, line.to, line.color);
}

// -------------------------------------------------------------------------- //

void
DebugDraw::Submit(const Vector2F& from, const Vector2F& to, Color color)
{
  // Flush if max limix reached
  if (mLineCount >= MAX_LINES) {
    Flush();
  }

  // From
  Vertex vertex;
  vertex.position = from;
  vertex.color = color;
  mLineData.push_back(vertex);

  // To
  vertex.position = to;
  vertex.color = color;
  mLineData.push_back(vertex);

  mLineCount++;
}

// -------------------------------------------------------------------------- //

void
DebugDraw::Submit(const DebugDraw::Rect& rect)
{
  Submit(rect.position, rect.position + Vector2F(0, rect.size.y), rect.color);
  Submit(rect.position, rect.position + Vector2F(rect.size.x, 0), rect.color);
  Submit(rect.position + Vector2F(0, rect.size.y),
         rect.position + Vector2F(rect.size.x, rect.size.y),
         rect.color);
  Submit(rect.position + Vector2F(rect.size.x, 0),
         rect.position + Vector2F(rect.size.x, rect.size.y),
         rect.color);
}

// -------------------------------------------------------------------------- //

void
DebugDraw::Submit(const DebugDraw::Circle& circle)
{
  u32 segments = circle.radius * 10;
  f32 radPerSeg = (2.0f * DIB_PI) / segments;

  Vector2F first(circle.position.x + circle.radius, circle.position.y);
  Vector2F last = first;
  for (u32 i = 1; i < segments; i++) {
    Vector2F current(circle.position.x + cos(radPerSeg * i) * circle.radius,
                     circle.position.y + sin(radPerSeg * i) * circle.radius);
    Submit(last, current, circle.color);
    last = current;
  }
  Submit(last, first, circle.color);
}

// -------------------------------------------------------------------------- //

void
DebugDraw::SetupBuffers()
{
  // Setup VAO
  glGenVertexArrays(1, &mVAO);
  glBindVertexArray(mVAO);
  mVertexBuffer.Bind();
  glEnableVertexAttribArray(VERTEX_ATTRIBUTE_POS);
  glVertexAttribPointer(VERTEX_ATTRIBUTE_POS,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(VERTEX_ATTRIBUTE_COL);
  glVertexAttribPointer(VERTEX_ATTRIBUTE_COL,
                        4,
                        GL_UNSIGNED_BYTE,
                        GL_TRUE,
                        sizeof(Vertex),
                        (void*)offsetof(Vertex, color));

  glBindVertexArray(0);
}

// -------------------------------------------------------------------------- //

void
DebugDraw::SetupShaders()
{
  // Load shaders
  mShaderProgram =
    ShaderManager::LoadProgram("debug_draw",
                               "./res/shaders/debug_draw.vs.glsl",
                               "./res/shaders/debug_draw.fs.glsl");
}

}