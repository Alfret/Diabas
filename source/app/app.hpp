#pragma once

// Library headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Diabas headers
#include "core/types.hpp"
#include "core/common.hpp"
#include "app/key.hpp"
#include "app/mouse.hpp"
#include "graphics/graphics.hpp"

// ========================================================================== //
// Application Declaration
// ========================================================================== //

namespace dib {

/** Main application class
 */
class Application
{
public:
  /** Application flags **/
  enum class Flag
  {
    /** No specific flags **/
    kNone = 0,
    /** Window should be made resizable **/
    kResizable = Bit(0)
  };

  /** Application descriptor **/
  struct Descriptor
  {
    /** Title of application **/
    String title;
    /** Width of application window **/
    u32 width;
    /** Height of application window **/
    u32 height;
  };

private:
  /** Window handle **/
  GLFWwindow* mWindow;

  /** Graphics interface **/
  Graphics* mGraphics;

  /** Whether application is running **/
  bool mRunning = false;

  /** Title of the window **/
  String mTitle;
  /** Width of the window **/
  u32 mWidth;
  /** Height of the window **/
  u32 mHeight;

public:
  /** Construct application from a descriptor **/
  explicit Application(const Descriptor& descriptor);

  /** Destroy application **/
  virtual ~Application();

  /** Run the application loop **/
  void Run();

  /** Exit application loop **/
  void Exit();

  /** Show window **/
  void Show() const;

  /** Hide window **/
  void Hide() const;

  /** Returns whether or not a key is down **/
  bool IsKeyDown(Key key) const;

  /** Returns whether or not a mouse button is down **/
  bool IsMouseDown(MouseButton button) const;

  /** Returns the mouse position **/
  void GetMousePosition(f64& x, f64& y) const;

  /** Returns the graphics interface **/
  Graphics& GetGraphics() const { return *mGraphics; }

  /** Returns the width of the application window **/
  u32 GetWidth() const { return mWidth; }

  /** Returns the height of the application window **/
  u32 GetHeight() const { return mHeight; }

  /** Called to update application **/
  virtual void Update(f64 delta) = 0;

  /** Called to render application **/
  virtual void Render() = 0;

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

  /** Returns the GLFW window handle **/
  GLFWwindow* GetSDLWindowHandle() const { return mWindow; }

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

};

}