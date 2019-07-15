#include "graphics/sprite_batch.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <cstddef>
#include "graphics/sprite.hpp"

// ========================================================================== //
// SpriteBatch Implementation
// ========================================================================== //

namespace dib::graphics {

SpriteBatch::SpriteBatch()
  : mVertexBuffer(8, nullptr, true, "SpriteBatchVB")
  , mIndexBuffer(8, {}, false, "SpriteBatchIB")
{
  SetupBuffers();
  SetupShaders();
}

// -------------------------------------------------------------------------- //

SpriteBatch::~SpriteBatch() {}

// -------------------------------------------------------------------------- //

void
SpriteBatch::NewFrame()
{
  mDrawCallCount = 0;
  mDrawSpriteCount = 0;
}

// -------------------------------------------------------------------------- //

void
SpriteBatch::Begin(const Camera* camera)
{
  mCamera = camera;
  mData.resize(0);
  mDataCount = 0;
}

// -------------------------------------------------------------------------- //

void
SpriteBatch::End()
{
  // Don't render if there are nothing to render
  if (mDataCount < 1) {
    return;
  }

  // Assert precondition
  AlfAssert(mCamera != nullptr, "'Begin' must have been called before 'End'");

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  mVertexBuffer.Upload(reinterpret_cast<const u8*>(mData.data()),
                       mData.size() * sizeof(Vertex));
  mCurrentTexture->Bind(0);
  mShaderProgram->Bind();
  mShaderProgram->SetUniformMatrix4("u_view_proj",
                                    mCamera->GetViewProjectMatrix());
  mShaderProgram->SetUniformS32("u_sampler", 0);
  glBindVertexArray(mVAO);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDrawElements(GL_TRIANGLES, mDataCount * 6, GL_UNSIGNED_INT, nullptr);

  mDrawCallCount++;
  mDrawSpriteCount += mDataCount;
}

// -------------------------------------------------------------------------- //

void
SpriteBatch::Flush()
{
  End();
  Begin(mCamera);
}

// -------------------------------------------------------------------------- //

void
SpriteBatch::Submit(const Sprite& sprite)
{
  // Flush if max limix reached
  if (mDataCount >= MAX_SPRITES) {
    Flush();
  }

  // Flush if different texture
  if (mCurrentTexture && mCurrentTexture != sprite.GetTexture()) {
    Flush();
  }
  mCurrentTexture = sprite.GetTexture();

  const Vector3F& position = sprite.GetPosition();
  const Vector2F& size = sprite.GetSize();
  const Vector2F& anchor = sprite.GetAnchor();
  const Vector2F& texMin = sprite.GetTexMin();
  const Vector2F& texMax = sprite.GetTexMax();

  // Bottom-left
  Vertex vertex;
  vertex.position = position;
  vertex.tint = alflib::Color::WHITE;
  vertex.uv = texMin;
  mData.push_back(vertex);

  // Top-left
  vertex.position = position;
  vertex.position.y += size.y;
  vertex.tint = alflib::Color::WHITE;
  vertex.uv = Vector2F(texMin.x, texMax.y);
  mData.push_back(vertex);

  // Top-right
  vertex.position = position;
  vertex.position.x += size.x;
  vertex.position.y += size.y;
  vertex.tint = alflib::Color::WHITE;
  vertex.uv = texMax;

  mData.push_back(vertex);
  // Bottom-right
  vertex.position = position;
  vertex.position.x += size.x;
  vertex.tint = alflib::Color::WHITE;
  vertex.uv = Vector2F(texMax.x, texMin.y);
  mData.push_back(vertex);

  mDataCount++;
}

// -------------------------------------------------------------------------- //

void
SpriteBatch::Submit(const std::shared_ptr<Texture>& texture,
                    Vector3F position,
                    Vector2F size,
                    alflib::Color color,
                    Vector2F texMin,
                    Vector2F texMax)
{
  // Flush if max limix reached
  if (mDataCount >= MAX_SPRITES) {
    Flush();
  }

  // Flush if different texture
  if (mCurrentTexture && mCurrentTexture != texture) {
    Flush();
  }
  mCurrentTexture = texture;

  // Bottom-left
  Vertex vertex;
  vertex.position = position;
  vertex.tint = color;
  vertex.uv = texMax;
  mData.push_back(vertex);

  // Top-left
  vertex.position = position;
  vertex.position.y += size.y;
  vertex.tint = color;
  vertex.uv = Vector2F(texMax.x, texMin.y);
  mData.push_back(vertex);

  // Top-right
  vertex.position = position;
  vertex.position.x += size.x;
  vertex.position.y += size.y;
  vertex.tint = color;
  vertex.uv = texMin;
  mData.push_back(vertex);

  // Bottom-right
  vertex.position = position;
  vertex.position.x += size.x;
  vertex.tint = color;
  vertex.uv = Vector2F(texMin.x, texMax.y);
  mData.push_back(vertex);

  mDataCount++;
}

// -------------------------------------------------------------------------- //

void
SpriteBatch::SetupBuffers()
{
  // Setup IBO
  std::vector<u32> indices;
  u32 indexOffset = 0;
  for (u32 i = 0; i < MAX_SPRITES; i++) {
    indices.push_back(indexOffset);
    indices.push_back(indexOffset + 1);
    indices.push_back(indexOffset + 2);
    indices.push_back(indexOffset);
    indices.push_back(indexOffset + 2);
    indices.push_back(indexOffset + 3);
    indexOffset += 4;
  }
  mIndexBuffer.Upload(indices, MAX_INDICES);

  // Setup VAO
  glGenVertexArrays(1, &mVAO);
  glBindVertexArray(mVAO);
  mVertexBuffer.Bind();
  glEnableVertexAttribArray(VERTEX_ATTRIBUTE_POS);
  glVertexAttribPointer(VERTEX_ATTRIBUTE_POS,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(VERTEX_ATTRIBUTE_TINT);
  glVertexAttribPointer(VERTEX_ATTRIBUTE_TINT,
                        4,
                        GL_UNSIGNED_BYTE,
                        GL_TRUE,
                        sizeof(Vertex),
                        (void*)offsetof(Vertex, tint));
  glEnableVertexAttribArray(VERTEX_ATTRIBUTE_UV);
  glVertexAttribPointer(VERTEX_ATTRIBUTE_UV,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*)offsetof(Vertex, uv));
  mIndexBuffer.Bind();
  glBindVertexArray(0);
  // Setup vertex attributes
}

// -------------------------------------------------------------------------- //

void
SpriteBatch::SetupShaders()
{
  // Load shaders
  mShaderProgram =
    ShaderManager::LoadProgram("sprite_batch",
                               "./res/shaders/sprite_batch_vs.glsl",
                               "./res/shaders/sprite_batch_fs.glsl");
}

}