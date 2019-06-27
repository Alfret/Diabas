#include "graphics/texture.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::graphics {

GLenum
ToGLTextureFormat(alflib::Image::Format format)
{
  switch (format) {
    case alflib::Image::Format::kUnknown:
      return GL_INVALID_ENUM;
    case alflib::Image::Format::kGrayscale:
      return GL_RED;
    case alflib::Image::Format::kRGBA:
      return GL_RGBA;
    case alflib::Image::Format::kRGB:
      return GL_RGB;
    case alflib::Image::Format::kBGRA:
      return GL_BGRA;
    case alflib::Image::Format::kBGR:
      return GL_BGR;
    default:
      return GL_INVALID_ENUM;
  }
}

}

// ========================================================================== //
// Texture Implementation
// ========================================================================== //

namespace dib::graphics {

Texture::Texture(const String& debugName)
{
  // Create texture
  glGenTextures(1, &mId);

  // Set name
  if (debugName.GetLength() > 0) {
    glObjectLabel(GL_TEXTURE,
                  mId,
                  static_cast<GLsizei>(debugName.GetSize()),
                  debugName.GetUTF8());
  }
}

// -------------------------------------------------------------------------- //

void
Texture::Load(const alflib::Image& image, bool generateMipmaps)
{
  LoadFromImage(image, generateMipmaps);
}

// -------------------------------------------------------------------------- //

void
Texture::Load(const Path& path, bool generateMipmaps)
{
  alflib::Image image;
  image.Load(path);
  LoadFromImage(image, generateMipmaps);
}

// -------------------------------------------------------------------------- //

void
Texture::Bind(u32 slot)
{
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, mId);
}

// -------------------------------------------------------------------------- //

void
Texture::LoadFromImage(const alflib::Image& image, bool generateMipmaps)
{
  Bind(0);

  // Set sampler parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_MIN_FILTER,
                  generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               ToGLTextureFormat(image.GetFormat()),
               image.GetWidth(),
               image.GetHeight(),
               0,
               ToGLTextureFormat(image.GetFormat()),
               GL_UNSIGNED_BYTE,
               image.GetData());
  glBindTexture(GL_TEXTURE_2D, 0);

  if (generateMipmaps) {
    glBindTexture(GL_TEXTURE_2D, mId);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

}