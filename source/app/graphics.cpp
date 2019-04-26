#include "app/graphics.hpp"
#include "core/assert.hpp"

#include "app/ogl/graphics_ogl.hpp"
#if defined(DIB_TARGET_WINDOWS)
# include "app/d3d11/graphics_d3d11.hpp"
#endif

// ========================================================================== //
// Graphics Implementation
// ========================================================================== //

namespace dib {

Graphics::Graphics(const Descriptor& descriptor)
{
  mImplementation = nullptr;
  switch (descriptor.api)
  {
    case API::OpenGL:
      mImplementation = new GraphicsOpenGL(descriptor);
      break;
    case API::D3D11:
      mImplementation = new GraphicsD3D11(descriptor);
      break;
  }
  DIB_ASSERT(mImplementation != nullptr, "Failed to create graphics interface");
}

Graphics::~Graphics()
{
  delete mImplementation;
}

}