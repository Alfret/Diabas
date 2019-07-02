#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "app/app.hpp"
#include "app/key.hpp"
#include "app/mouse.hpp"

// ========================================================================== //
// AppClient Declaration
// ========================================================================== //

namespace dib::app {

/** Client application **/
class AppClient : public App
{
public:
  /** Client application descriptor **/
  struct Descriptor
  {
    /** Title **/
    String title;
    /** Width of window **/
    u32 width;
    /** Height of window **/
    u32 height;

    /** Whether to enable V-sync **/
    bool enableVSync;
  };

private:
  /** GLFW window **/
  GLFWwindow* mWindow;

  /** Window title **/
  String mTitle;
  /** Window width **/
  u32 mWidth;
  /** Window height **/
  u32 mHeight;

  /** Whether application is running **/
  bool mRunning = false;

  /** Whether mouse is grabbed by application **/
  bool mMouseGrabbed = false;

public:
  /** Construct client application **/
  explicit AppClient(const Descriptor& descriptor);

  /** Destruct **/
  ~AppClient() override;

  /** Run client application **/
  void Run() override;

  /** Exit application **/
  void Exit() override;

  /** Show window **/
  void Show() const;

  /** Hide window **/
  void Hide() const;

  /** Toggle fullscreen **/
  void ToggleFullscreen();

  /** Called to render application **/
  virtual void Render() = 0;

  /** Returns whether or not a key is down **/
  bool IsKeyDown(Key key) const;

  /** Returns whether or not a mouse button is down **/
  bool IsMouseDown(MouseButton button) const;

  /** Returns the mouse position **/
  void GetMousePosition(f64& x, f64& y) const;

  /** Called when a key has been pressed **/
  virtual void OnKeyPressed([[maybe_unused]] Key key) {}

  /** Called when a key has been released **/
  virtual void OnKeyReleased([[maybe_unused]] Key key) {}

  /** Called when a mouse button has been pressed **/
  virtual void OnMousePressed([[maybe_unused]] MouseButton button) {}

  /** Called when a mouse button has been released **/
  virtual void OnMouseReleased([[maybe_unused]] MouseButton button) {}

  /** Called when the mouse has been moved **/
  virtual void OnMouseMoved([[maybe_unused]] f64 x, [[maybe_unused]] f64 y) {}

  /** Called when the mouse wheel has been scrolled **/
  virtual void OnMouseScroll([[maybe_unused]] f64 deltaX,
                             [[maybe_unused]] f64 deltaY)
  {}

  /** Called when the application window has been resized **/
  virtual void OnWindowResize([[maybe_unused]] u32 width,
                              [[maybe_unused]] u32 height)
  {}

  /** Returns the title **/
  const String& GetTitle() const { return mTitle; }

  /** Returns the window width **/
  u32 GetWidth() const { return mWidth; }

  /** Returns the window height **/
  u32 GetHeight() const { return mHeight; }

  /** Returns the underlying GLFW window **/
  GLFWwindow* GetWindow() const { return mWindow; }

private:
  /** GLFW error callback **/
  static void ErrorCallback(int error, const char8* description);

  /** GLFW key callback **/
  static void KeyCallback(GLFWwindow* window,
                          int key,
                          int scancode,
                          int action,
                          int mods);

  /** GLFW text input callback **/
  static void CharCallback(GLFWwindow* window, unsigned int codepoint);

  /** GLFW mouse button callback **/
  static void MouseButtonCallback(GLFWwindow* window,
                                  int button,
                                  int action,
                                  int mods);

  /** GLFW mouse position callback **/
  static void MousePosCallback(GLFWwindow* window, f64 x, f64 y);

  /** GLFW mouse scroll callback **/
  static void MouseScrollCallback(GLFWwindow* window, f64 x, f64 y);

  /** GLFW window resize callback **/
  static void ResizeCallback(GLFWwindow* window, int width, int height);

  /** OpenGL debug callback **/
  static void DebugCallbackGL(GLenum source,
                              GLenum type,
                              GLuint id,
                              GLenum severity,
                              GLsizei length,
                              const GLchar* message,
                              void* userParam);
};

}