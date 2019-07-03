#include "graphics/index_buffer.hpp"

// ========================================================================== //
// VertexBuffer Implementation
// ========================================================================== //

namespace dib::graphics {

IndexBuffer::IndexBuffer(u32 count,
                         const std::vector<u32>& indices,
                         bool isDynamic,
                         String debugName)
  : mCount(0)
  , mIsDynamic(isDynamic)
{
  glGenBuffers(1, &mId);
  Upload(indices, count, 0);

  // Set debug name
  if (debugName.GetLength() > 0) {
    glObjectLabel(GL_BUFFER,
                  mId,
                  static_cast<GLsizei>(debugName.GetSize()),
                  debugName.GetUTF8());
  }
}

// -------------------------------------------------------------------------- //

void
IndexBuffer::Upload(const std::vector<u32>& indices, u32 count, u32 offset)
{
  Bind();
  if (count + offset > mCount) {
    Resize(count + offset);
  }
  if (count > 0) {
    glBufferSubData(
      GL_ELEMENT_ARRAY_BUFFER, offset, sizeof(u32) * count, indices.data());
  }
}

// -------------------------------------------------------------------------- //

void
IndexBuffer::Resize(u32 count)
{
  Bind();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(u32) * count,
               nullptr,
               mIsDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

// -------------------------------------------------------------------------- //

void
IndexBuffer::Bind()
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
}

}