#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <glad/glad.h>
#include <alflib/graphics/image.hpp>
#include "core/types.hpp"

// ========================================================================== //
// Texture Declaration
// ========================================================================== //

namespace dib::graphics {

/** Texture class **/
class Texture
{
private:
  /** Texture ID **/
  GLuint mId;
  /** Debug name **/
  String mDebugName;

public:
  /** Construct texture **/
  explicit Texture(const String& debugName = "");

  /** Load texture from image **/
  void Load(const alflib::Image& image, bool generateMipmaps = false);

  /** Load texture from path **/
  void Load(const Path& path, bool generateMipmaps = false);

  /** Bind texture to slot **/
  void Bind(u32 slot = 0);

  /** Returns the texture id **/
  GLuint GetID() const { return mId; }

private:
  /** Load data from image **/
  void LoadFromImage(const alflib::Image& image, bool generateMipmaps);
};

}
