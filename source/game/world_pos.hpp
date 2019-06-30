#pragma once

// ========================================================================== //
// WorldPos Declaration
// ========================================================================== //

namespace dib::game {

/** Class that represents a position in the world. Each unit is 0.5m, which
 * means each block is 1 unit wide **/
class WorldPos
{
private:
  /** X position **/
  u32 mX;
  /** Y position **/
  u32 mY;

public:
  /** Construct a world position from the x and y coordinates **/
  constexpr WorldPos(u32 x, u32 y)
    : mX(x)
    , mY(y)
  {}

  /** Returns the X position **/
  u32& X() { return mX; }

  /** Returns the X position **/
  const u32& X() const { return mX; }

  /** Returns the Y position **/
  u32& Y() { return mY; }

  /** Returns the Y position **/
  const u32& Y() const { return mY; }
};

}