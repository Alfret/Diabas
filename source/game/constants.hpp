#pragma once

// ========================================================================== //
// Constants
// ========================================================================== //

namespace dib::game {

/** Size of a tile in pixels **/
static constexpr u32 TILE_SIZE = 16;

/** Size of a wall in pixels **/
static constexpr u32 WALL_SIZE = 16;

/** ID for the "Mod" called "builtin". This fake mod is where all things created
 * in native code belongs **/
static constexpr char BUILTIN_MOD_NAME[] = "builtin";

}