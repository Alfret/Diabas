#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"

// ========================================================================== //
// Color Declaration
// ========================================================================== //

namespace dib::graphics {

/** RGBA color **/
class Color
{
public:
  /** Maximum color channel value **/
  static constexpr f32 VALUE_MAX = std::numeric_limits<u8>::max();

private:
  /** Color channels **/
  f32 mRed, mGreen, mBlue, mAlpha;

public:
  constexpr Color(s32 r, s32 g, s32 b, s32 a = static_cast<s32>(VALUE_MAX))
    : mRed(r / VALUE_MAX)
    , mGreen(g / VALUE_MAX)
    , mBlue(b / VALUE_MAX)
    , mAlpha(a / VALUE_MAX)
  {}

  constexpr Color(f32 r, f32 g, f32 b, f32 a = 1.0f)
    : mRed(r * VALUE_MAX)
    , mGreen(g * VALUE_MAX)
    , mBlue(b * VALUE_MAX)
    , mAlpha(a * VALUE_MAX)
  {}

  /** Red channel **/
  f32& Red() { return mRed; }

  /** Red channel **/
  const f32& Red() const { return mRed; }

  /** Green channel **/
  f32& Green() { return mGreen; }

  /** Green channel **/
  const f32& Green() const { return mGreen; }

  /** Blue channel **/
  f32& Blue() { return mBlue; }

  /** Blue channel **/
  const f32& Blue() const { return mBlue; }

  /** Alpha channel **/
  f32& Alpha() { return mAlpha; }

  /** Alpha channel **/
  const f32& Alpha() const { return mAlpha; }

public:
  /** Returns the color 'Cornflower Blue' **/
  static constexpr Color CORNFLOWER_BLUE() { return Color(100, 149, 237); }
};

}