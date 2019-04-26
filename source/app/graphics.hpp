#pragma once

#include "SDL.h"
#include "core/string.hpp"
#include "core/types.hpp"

// ========================================================================== //
// GraphicsImpl Declaration
// ========================================================================== //

namespace dib {

class GraphicsImpl
{
public:
  virtual ~GraphicsImpl() = default;

  virtual const String& DeviceName() const = 0;

  virtual void SetClearColor(f32 r, f32 g, f32 b, f32 a) = 0;

  virtual void Clear() = 0;

  virtual void Present() = 0;

};

}

// ========================================================================== //
// Graphics Declaration
// ========================================================================== //

namespace dib {

/** Graphics interface **/
class Graphics
{
public:
  /** Graphics APIs supported **/
  enum class API
  {
    OpenGL, //< OpenGL graphics API
    D3D11   //< Direct3D 11 graphics API
  };

  /** Graphics descriptor **/
  struct Descriptor
  {
    API api;
    SDL_Window* window;
  };

private:
  /** Pointer to graphics implementation **/
  GraphicsImpl* mImplementation;

public:
  /** Create grahics interface from descriptor **/
  Graphics(const Descriptor& descriptor);

  /** Destroy graphics interface **/
  ~Graphics();

  /** Returns the name of the graphics device **/
  const String& DeviceName() const { return mImplementation->DeviceName(); }

  /** Set the clear color **/
  void SetClearColor(f32 r, f32 g, f32 b, f32 a)
  {
    mImplementation->SetClearColor(r, g, b, a);
  }

  /** Clear render target **/
  void Clear() { mImplementation->Clear(); }

  /** Present to the window **/
  void Present() { mImplementation->Present(); }

};

}