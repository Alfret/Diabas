#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/tile/tile.hpp"

// ========================================================================== //
// TileVariant Declaration
// ========================================================================== //

namespace dib::game {

/** Variant tile **/
class TileVariant : public Tile
{
public:
  static constexpr u8 kTopLeft = 1u;
  static constexpr u8 kTop = 2u;
  static constexpr u8 kTopRight = 4u;
  static constexpr u8 kLeft = 8u;
  static constexpr u8 kRight = 16u;
  static constexpr u8 kBottomLeft = 32u;
  static constexpr u8 kBottom = 64u;
  static constexpr u8 kBottomRight = 128u;

public:
  TileVariant(const ResourcePath& resourcePath, const String& translationKey);

  u32 GetResourceIndex(World& world, WorldPos pos) override;

private:
  u32 LookupResourceIndex(u8 mask);
};
}