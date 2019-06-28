#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <glad/glad.h>
#include "core/types.hpp"

// ========================================================================== //
// IndexBuffer Declaration
// ========================================================================== //

namespace dib::graphics {

/** Index buffer **/
class IndexBuffer
{
private:
  /** Buffer ID **/
  GLuint mId;
  /** Number of indices in buffer **/
  u32 mCount;
  /** Whether buffer is dynamic **/
  bool mIsDynamic;

public:
  /** Construct index buffer **/
  explicit IndexBuffer(u32 count,
                       const std::vector<u32>& indices = {},
                       bool isDynamic = false,
                       String debugName = "");

  /** Upload data. Buffer is resized to fit data  **/
  void Upload(const std::vector<u32>& indices, u32 count, u32 offset = 0);

  /** Resize buffer **/
  void Resize(u32 count);

  /** Bind buffer **/
  void Bind();
};

}