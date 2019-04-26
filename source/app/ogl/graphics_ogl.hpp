#pragma once

#include <SDL.h>
#include "app/graphics.hpp"
#include "core/platform/platform.hpp"

// ========================================================================== //
// GraphicsD3D11 Declaration
// ========================================================================== //

namespace dib {

class GraphicsOpenGL : public GraphicsImpl
{
private:
  /** SDL OpenGL context **/
  SDL_GLContext mHandle;
  /** Reference to window **/
  SDL_Window* mWindowRef;

  /** Device name **/
  String mDeviceName;

public:
  GraphicsOpenGL(const Graphics::Descriptor& descriptor);

  ~GraphicsOpenGL();

  const String& DeviceName() const override { return mDeviceName; }

  virtual void SetClearColor(f32 r, f32 g, f32 b, f32 a);

  void Clear() override;

  void Present() override;

};

}