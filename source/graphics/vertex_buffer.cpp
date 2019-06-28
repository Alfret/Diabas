#include "graphics/vertex_buffer.hpp"

// ========================================================================== //
// VertexBuffer Implementation
// ========================================================================== //

namespace dib::graphics {

VertexBuffer::VertexBuffer(u32 size,
                           u8* initialData,
                           bool isDynamic,
                           String debugName)
  : mSize(0)
  , mIsDynamic(isDynamic)
{
  glGenBuffers(1, &mId);
  Upload(initialData, size, 0);

  // Set debug name
  if (debugName.GetLength() > 0) {
    glObjectLabel(GL_PROGRAM,
                  mId,
                  static_cast<GLsizei>(debugName.GetSize()),
                  debugName.GetUTF8());
  }
}

// -------------------------------------------------------------------------- //

void
VertexBuffer::Upload(const u8* data, u32 size, u32 offset)
{
  Bind();
  if (size + offset > mSize) {
    Resize(size + offset);
  }
  if (data) {
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
  }
}

// -------------------------------------------------------------------------- //

void
VertexBuffer::Resize(u32 size)
{
  Bind();
  glBufferData(GL_ARRAY_BUFFER,
               size,
               nullptr,
               mIsDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

// -------------------------------------------------------------------------- //

void
VertexBuffer::Bind()
{
  glBindBuffer(GL_ARRAY_BUFFER, mId);
}

}