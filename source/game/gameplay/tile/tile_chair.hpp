#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/tile/tile.hpp"

// ========================================================================== //
// TileChair Declaration
// ========================================================================== //

namespace dib::game {

/** Chair tile **/
class TileChair : public Tile
{
public:
  TileChair(const ResourcePath& resourcePath, const String& translationKey);

  bool IsMultiTile(World& world, WorldPos pos) override;

  bool CanPlaceMultiTile(World& world, WorldPos pos) override;

  bool PlaceMultiTile(World& world, WorldPos pos) override;

  bool KillMultiTile(World& world, WorldPos pos) override;

  u32 GetResourceIndex(World& world, WorldPos pos) override;
};

}