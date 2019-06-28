#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <glad/glad.h>
#include "core/types.hpp"

// ========================================================================== //
// VertexBuffer Declaration
// ========================================================================== //

namespace dib::graphics {

/** Vertex buffer **/
class VertexBuffer
{
private:
  /** Buffer ID **/
  GLuint mId;
  /** Buffer size **/
  u32 mSize;
  /** Whether buffer is dynamic **/
  bool mIsDynamic;

public:
  /** Construct vertex buffer **/
  explicit VertexBuffer(u32 size,
                        u8* initialData = nullptr,
                        bool isDynamic = false,
                        String debugName = "");

  /** Upload data. Buffer is resized to fit data  **/
  void Upload(const u8* data, u32 size, u32 offset = 0);

  /** Resize buffer **/
  void Resize(u32 size);

  /** Bind buffer **/
  void Bind();
};

}