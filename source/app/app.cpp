#include "app/app.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/assert.hpp"
#include "core/macros.hpp"

// ========================================================================== //
// Application Implementation
// ========================================================================== //

namespace dib {

Application::Application(const Descriptor& descriptor)
  : mTitle(descriptor.title)
  , mWidth(descriptor.width)
  , mHeight(descriptor.height)
{
  // Initialize GLFW
  const int result = glfwInit();
  DIB_ASSERT(result == GLFW_TRUE, "Failed to initialize SDL2");

  // Create window
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  mWindow =
    glfwCreateWindow(mWidth, mHeight, mTitle.CString(), nullptr, nullptr);
  DIB_ASSERT(mWindow != nullptr, "Failed to create window");

  // Setup graphics
  mGraphics = new Graphics(mWindow);
  DIB_ASSERT(mGraphics != nullptr,
             "Failed to create application graphics interface");
}

// -------------------------------------------------------------------------- //

Application::~Application()
{
  glfwDestroyWindow(mWindow);
  glfwTerminate();
}

// -------------------------------------------------------------------------- //

void
Application::Run()
{
  mRunning = true;
  f64 timeLast = glfwGetTime();
  while (mRunning) {
    // Update time variables
    const f64 timeCurrent = glfwGetTime();
    const f64 timeDelta = timeCurrent - timeLast;
    timeLast = timeCurrent;

    // Check if window has been requested to close by user
    if (glfwWindowShouldClose(mWindow)) {
      mRunning = false;
      break;
    }

    // Update
    glfwPollEvents();
    Update(timeDelta);

    // Render
    mGraphics->Clear();
    Render();
    mGraphics->Present();
  }
}

// -------------------------------------------------------------------------- //

void
Application::Exit()
{
  mRunning = false;
}

// -------------------------------------------------------------------------- //

void
Application::Show() const
{
  glfwShowWindow(mWindow);
}

// -------------------------------------------------------------------------- //

void
Application::Hide() const
{
  glfwHideWindow(mWindow);
}

// -------------------------------------------------------------------------- //

bool
Application::IsKeyDown(Key key) const
{
  return glfwGetKey(mWindow, int(key)) == GLFW_PRESS;
}

// -------------------------------------------------------------------------- //

bool
Application::IsMouseDown(MouseButton button) const
{
  return glfwGetMouseButton(mWindow, int(button)) == GLFW_PRESS;
}

// -------------------------------------------------------------------------- //

void
Application::GetMousePosition(f64& x, f64& y) const
{
  glfwGetCursorPos(mWindow, &x, &y);
}

// -------------------------------------------------------------------------- //

void
Application::ErrorCallback(int error, const char8* description)
{
  //Logger::Default().Error("GLFW error ({} - {})", error, description);
}

// -------------------------------------------------------------------------- //

void
Application::KeyCallback(GLFWwindow* window,
                         int key,
                         int scancode,
                         int action,
                         int mods)
{
  DIB_UNUSED(scancode);
  DIB_UNUSED(mods);
  Application* app =
    static_cast<Application*>(glfwGetWindowUserPointer(window));
  if (app) {
    if (action == GLFW_PRESS) {
      app->OnKeyPressed(static_cast<Key>(key));
    } else {
      app->OnKeyReleased(static_cast<Key>(key));
    }
  }
}

// -------------------------------------------------------------------------- //

void
Application::CharCallback(GLFWwindow* window, unsigned codepoint)
{
  DIB_UNUSED(window);
  DIB_UNUSED(codepoint);
}

// -------------------------------------------------------------------------- //

void
Application::MouseButtonCallback(GLFWwindow* window,
                                 int button,
                                 int action,
                                 int mods)
{
  DIB_UNUSED(mods);
  Application* app =
    static_cast<Application*>(glfwGetWindowUserPointer(window));
  if (app) {
    if (action == GLFW_PRESS) {
      app->OnMousePressed(static_cast<MouseButton>(button));
    } else {
      app->OnMouseReleased(static_cast<MouseButton>(button));
    }
  }
}

// -------------------------------------------------------------------------- //

void
Application::MousePosCallback(GLFWwindow* window, f64 x, f64 y)
{
  Application* app =
    static_cast<Application*>(glfwGetWindowUserPointer(window));
  if (app) {
    app->OnMouseMoved(x, y);
  }
}

// -------------------------------------------------------------------------- //

void
Application::MouseScrollCallback(GLFWwindow* window, f64 x, f64 y)
{
  Application* app =
    static_cast<Application*>(glfwGetWindowUserPointer(window));
  if (app) {
    app->OnMouseScroll(x, y);
  }
}

// -------------------------------------------------------------------------- //

void
Application::ResizeCallback(GLFWwindow* window, int width, int height)
{
  Application* app =
    static_cast<Application*>(glfwGetWindowUserPointer(window));
  if (app) {
    const u32 _width = static_cast<u32>(width);
    const u32 _height = static_cast<u32>(height);
    app->mWidth = _width;
    app->mHeight = _height;
    if (_width && _height) {
      app->GetGraphics().Resize(_width, _height);
    }
    app->OnWindowResize(_width, _height);
  }
}

}
