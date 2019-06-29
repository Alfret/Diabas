#include "app/client/app_client.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>
#include <dutil/stopwatch.hpp>

#include "core/assert.hpp"
#include "core/macros.hpp"
#include "app/client/imgui/imgui.h"
#include "app/client/imgui/imgui_impl_glfw_gl3.h"
#include "graphics/shader.hpp"

// ========================================================================== //
// Private Functions
// ========================================================================== //

namespace dib::app {

static void
AppClientSetImGuiStyle(ImGuiStyle& style)
{
  ImVec4* colors = style.Colors;

  style.WindowRounding = 2.0f; // Radius of window corners rounding. Set to
  // 0.0f to have rectangular windows
  style.ScrollbarRounding =
    3.0f;                    // Radius of grab corners rounding for scrollbar
  style.GrabRounding = 2.0f; // Radius of grabs corners rounding. Set to 0.0f
  // to have rectangular slider grabs.
  style.AntiAliasedLines = true;
  style.AntiAliasedShapes = true;
  style.WindowRounding = 2;
  style.ChildWindowRounding = 2;
  style.ScrollbarSize = 16;
  style.ScrollbarRounding = 3;
  style.GrabRounding = 2;
  style.ItemSpacing.x = 10;
  style.ItemSpacing.y = 4;
  style.IndentSpacing = 22;
  style.FramePadding.x = 6;
  style.FramePadding.y = 4;
  style.Alpha = 1.0f;
  style.FrameRounding = 3.0f;

  colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
  // colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f,
  // 0.00f);
  colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.93f, 0.93f, 0.93f, 0.98f);
  colors[ImGuiCol_Border] = ImVec4(0.71f, 0.71f, 0.71f, 0.08f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.71f, 0.71f, 0.71f, 0.55f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.94f, 0.94f, 0.94f, 0.55f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.71f, 0.78f, 0.69f, 0.98f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.82f, 0.78f, 0.78f, 0.51f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.61f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.90f, 0.90f, 0.90f, 0.30f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.92f, 0.92f, 0.78f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.184f, 0.407f, 0.193f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_Button] = ImVec4(0.71f, 0.78f, 0.69f, 0.40f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.725f, 0.805f, 0.702f, 1.00f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.793f, 0.900f, 0.836f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.71f, 0.78f, 0.69f, 0.31f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.71f, 0.78f, 0.69f, 0.80f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.71f, 0.78f, 0.69f, 1.00f);
  colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
  colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.45f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
  colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

}

// ========================================================================== //
// AppClient
// ========================================================================== //

namespace dib::app {

AppClient::AppClient(const AppClient::Descriptor& descriptor)
  : mTitle(descriptor.title)
  , mWidth(descriptor.width)
  , mHeight(descriptor.height)
{
  // Initialize GLFW
  int result = glfwInit();
  DIB_ASSERT(result == GLFW_TRUE, "Failed to initialize SDL2");

  // Create window
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  mWindow =
    glfwCreateWindow(mWidth, mHeight, mTitle.GetUTF8(), nullptr, nullptr);
  DIB_ASSERT(mWindow != nullptr, "Failed to create window");
  glfwSetWindowUserPointer(mWindow, this);
  glfwSetErrorCallback(ErrorCallback);
  glfwSetKeyCallback(mWindow, KeyCallback);
  glfwSetCharCallback(mWindow, CharCallback);
  glfwSetMouseButtonCallback(mWindow, MouseButtonCallback);
  glfwSetCursorPosCallback(mWindow, MousePosCallback);
  glfwSetScrollCallback(mWindow, MouseScrollCallback);
  glfwSetFramebufferSizeCallback(mWindow, ResizeCallback);

  // Setup graphics
  glfwMakeContextCurrent(mWindow);
  result = gladLoadGL();
  DIB_ASSERT(result != 0, "Failed to initialize GLAD");
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback((GLDEBUGPROC)AppClient::DebugCallbackGL, this);

  // Initialize ImGui
  ImGui_ImplGlfwGL3_Init(mWindow, false);
  AppClientSetImGuiStyle(ImGui::GetStyle());
}

// -------------------------------------------------------------------------- //

AppClient::~AppClient()
{
  graphics::ShaderManager::UnloadAll();

  ImGui_ImplGlfwGL3_Shutdown();
  glfwDestroyWindow(mWindow);
  glfwTerminate();
}

// -------------------------------------------------------------------------- //

void
AppClient::Run()
{
  // Create and start stopwatch
  dutil::Stopwatch sw;
  sw.Start();

  // Run main loop
  f64 timeLast = sw.fnow_s();
  mRunning = true;
  while (mRunning) {
    // Update time variables
    const f64 timeCurrent = sw.fnow_s();
    const f64 timeDelta = timeCurrent - timeLast;
    timeLast = timeCurrent;

    // Check if window has been requested to close by user
    if (glfwWindowShouldClose(mWindow)) {
      mRunning = false;
      break;
    }

    // Update ImGui
    ImGui_ImplGlfwGL3_NewFrame(mMouseGrabbed);

    // Update
    glfwPollEvents();
    Update(timeDelta);

    // Render
    Render();
    ImGui::Render();
    glfwSwapBuffers(mWindow);
  }
}

// -------------------------------------------------------------------------- //

void
AppClient::Exit()
{
  mRunning = false;
}

// -------------------------------------------------------------------------- //

void
AppClient::Show() const
{
  glfwShowWindow(mWindow);
}

// -------------------------------------------------------------------------- //

void
AppClient::Hide() const
{
  glfwHideWindow(mWindow);
}

// -------------------------------------------------------------------------- //

void
AppClient::ToggleFullscreen()
{}

// -------------------------------------------------------------------------- //

bool
AppClient::IsKeyDown(Key key) const
{
  return glfwGetKey(mWindow, int(key)) == GLFW_PRESS;
}

// -------------------------------------------------------------------------- //

bool
AppClient::IsMouseDown(MouseButton button) const
{
  return glfwGetMouseButton(mWindow, int(button)) == GLFW_PRESS;
}

// -------------------------------------------------------------------------- //

void
AppClient::GetMousePosition(f64& x, f64& y) const
{
  glfwGetCursorPos(mWindow, &x, &y);
  y = mHeight - y;
}

// -------------------------------------------------------------------------- //

void
AppClient::ErrorCallback(int error, const char8* description)
{
  DLOG_ERROR("[GLFW]: {} ({})", description, error);
}

// -------------------------------------------------------------------------- //

void
AppClient::KeyCallback(GLFWwindow* window,
                       int key,
                       int scancode,
                       int action,
                       int mods)
{
  // First let ImGui handle the key callback
  ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
  if (ImGui::GetIO().WantCaptureKeyboard) {
    return;
  }

  // Then let the application get a chance to handle it
  auto app = static_cast<AppClient*>(glfwGetWindowUserPointer(window));
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
AppClient::CharCallback(GLFWwindow* window, unsigned int codepoint)
{
  // Forward event to GUI
  ImGui_ImplGlfwGL3_CharCallback(window, codepoint);
  if (ImGui::GetIO().WantTextInput) {
    return;
  }
}

// -------------------------------------------------------------------------- //

void
AppClient::MouseButtonCallback(GLFWwindow* window,
                               int button,
                               int action,
                               int mods)
{
  // Forward event to GUI
  ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
  if (ImGui::GetIO().WantCaptureMouse) {
    return;
  }

  // Then let app handle it
  auto app = static_cast<AppClient*>(glfwGetWindowUserPointer(window));
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
AppClient::MousePosCallback(GLFWwindow* window, f64 x, f64 y)
{
  // Let ImGui handle mouse
  if (ImGui::GetIO().WantCaptureMouse) {
    return;
  }

  // Then let app handle it
  auto app = static_cast<AppClient*>(glfwGetWindowUserPointer(window));
  if (app) {
    app->OnMouseMoved(x, y);
  }
}

// -------------------------------------------------------------------------- //

void
AppClient::MouseScrollCallback(GLFWwindow* window, f64 x, f64 y)
{
  // Let ImGui handle mouse
  ImGui_ImplGlfwGL3_ScrollCallback(window, x, y);
  if (ImGui::GetIO().WantCaptureMouse) {
    return;
  }

  // Then let app handle
  auto app = static_cast<AppClient*>(glfwGetWindowUserPointer(window));
  if (app) {
    app->OnMouseScroll(x, y);
  }
}

// -------------------------------------------------------------------------- //

void
AppClient::ResizeCallback(GLFWwindow* window, int width, int height)
{
  auto app = static_cast<AppClient*>(glfwGetWindowUserPointer(window));
  if (app) {
    const u32 _width = static_cast<u32>(width);
    const u32 _height = static_cast<u32>(height);
    app->mWidth = _width;
    app->mHeight = _height;
    if (_width && _height) {
      glViewport(0, 0, _width, _height);
    }
    app->OnWindowResize(_width, _height);
  }
}

// -------------------------------------------------------------------------- //

void
AppClient::DebugCallbackGL(GLenum source,
                           GLenum type,
                           GLuint id,
                           GLenum severity,
                           GLsizei length,
                           const GLchar* message,
                           void* userParam)
{
  auto app = static_cast<AppClient*>(userParam);
  if (type == GL_DEBUG_TYPE_ERROR) {
    DLOG_ERROR("OpenGL: {}", message);
  } else if (type == GL_DEBUG_TYPE_PERFORMANCE ||
             type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR ||
             type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) {
    DLOG_WARNING("OpenGL: {}", message);
  } else {
    // DLOG_VERBOSE("OpenGL: {}", message);
  }
}

}