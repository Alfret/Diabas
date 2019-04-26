#include "app/app.hpp"
#include "core/assert.hpp"

// ========================================================================== //
// Application Implementation
// ========================================================================== //

namespace dib {

Application::Application(const Descriptor& descriptor)
{
  // Initialize SDL2
  int result = SDL_Init(SDL_INIT_VIDEO);
  DIB_ASSERT(result == 0, "Failed to initialize SDL2");

  // Create window
  Uint32 flags = SDL_WINDOW_ALLOW_HIGHDPI;
  if (descriptor.api == Graphics::API::OpenGL) {
    flags |= SDL_WINDOW_OPENGL;
  }
  mWindow = SDL_CreateWindow(descriptor.title.CString(),
                             SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED,
                             descriptor.width,
                             descriptor.height,
                             flags);
  DIB_ASSERT(mWindow != nullptr, "Failed to create window");

  // Setup graphics
  Graphics::Descriptor graphicsDescriptor{};
  graphicsDescriptor.api = descriptor.api;
  graphicsDescriptor.window = mWindow;
  mGraphics = new Graphics(graphicsDescriptor);
  DIB_ASSERT(mGraphics != nullptr,
             "Failed to create application graphics interface");
}

// -------------------------------------------------------------------------- //

Application::~Application()
{
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
}

// -------------------------------------------------------------------------- //

void
Application::Run()
{
  bool running = true;
  SDL_Event event;
  while (running) {

    // Poll all events
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          running = false;
        default:
          break;
      }
    }

    // Update

    // Render
    mGraphics->Clear();

    // Present
    mGraphics->Present();
  }
}

}