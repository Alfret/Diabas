#pragma once

// ========================================================================== //
// MouseButton Enumeration
// ========================================================================== //

namespace dib {

/** Enumeration of mouse buttons **/
enum class MouseButton
{
  kMouseButton1 = GLFW_MOUSE_BUTTON_1,
  kMouseButton2 = GLFW_MOUSE_BUTTON_2,
  kMouseButton3 = GLFW_MOUSE_BUTTON_3,
  kMouseButton4 = GLFW_MOUSE_BUTTON_4,
  kMouseButton5 = GLFW_MOUSE_BUTTON_5,
  kMouseButton6 = GLFW_MOUSE_BUTTON_6,
  kMouseButton7 = GLFW_MOUSE_BUTTON_7,
  kMouseButton8 = GLFW_MOUSE_BUTTON_8,
  kMouseButtonLast = kMouseButton8,
  /** Left mouse button **/
  kMouseButtonLeft = kMouseButton1,
  /** Right mouse button **/
  kMouseButtonRight = kMouseButton2,
  /** Middle mouse button (Scroll wheel) **/
  kMouseButtonMiddle = kMouseButton3
};

}