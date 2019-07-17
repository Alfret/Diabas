#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/common.hpp"

// ========================================================================== //
// ToolType Enumeration
// ========================================================================== //

namespace dib::game {

/** Enumeration of tool types. These can be combined together in bit-masks **/
enum class ToolType : u32
{
  /** Pickaxe **/
  kPickaxe = Bit(0),
  kAxe = Bit(1),
  kSpade = Bit(2),
  kCustom = Bit(3),
  kAll = kPickaxe | kAxe | kSpade | kCustom
};
DIB_ENUM_CLASS_OPERATORS(inline, ToolType, u32)

}