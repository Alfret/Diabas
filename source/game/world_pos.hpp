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
  constexpr WorldPos(u32 x = 0, u32 y = 0)
    : mX(x)
    , mY(y)
  {}

  /** Left position **/
  WorldPos Left() const { return WorldPos{ mX - 1, mY }; }

  /** Right position **/
  WorldPos Right() const { return WorldPos{ mX + 1, mY }; }

  /** Top position **/
  WorldPos Top() const { return WorldPos{ mX, mY + 1 }; }

  /** Bottom position **/
  WorldPos Bottom() const { return WorldPos{ mX, mY - 1 }; }

  /** Top-left position **/
  WorldPos TopLeft() const { return WorldPos{ mX - 1, mY + 1 }; }

  /** Top-right position **/
  WorldPos TopRight() const { return WorldPos{ mX + 1, mY + 1 }; }

  /** Bottom-left position **/
  WorldPos BottomLeft() const { return WorldPos{ mX - 1, mY - 1 }; }

  /** Bottom-right position **/
  WorldPos BottomRight() const { return WorldPos{ mX + 1, mY - 1 }; }

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