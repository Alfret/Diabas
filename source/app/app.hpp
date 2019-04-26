#pragma once

#include <functional>
#include <SDL.h>
#include "core/types.hpp"
#include "app/graphics.hpp"

// ========================================================================== //
// Application Declaration
// ========================================================================== //

namespace dib {

/** Main application class
 */
class Application
{
public:
  /** Application descriptor **/
  struct Descriptor
  {
    /** Title of application **/
    String title;
    /** Width of application window **/
    u32 width;
    /** Height of application window **/
    u32 height;

    /** Graphics API to use **/
    Graphics::API api;
  };

private:
  /** Window handle **/
  SDL_Window* mWindow;

  /** Graphics interface **/
  Graphics *mGraphics;

public:
  /** Construct application from a descriptor **/
  Application(const Descriptor& descriptor);

  /** Destroy application **/
  ~Application();

  /** Run the application loop **/
  void Run();

  /** Returns the SDL window handle **/
  SDL_Window* GetSDLWindowHandle() const { return mWindow; }

  /** Returns the graphics interface of the application **/
  Graphics& GetGraphics() const { return *mGraphics; }

};

}