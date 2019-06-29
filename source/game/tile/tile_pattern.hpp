#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <vector>

#include "game/tile/tile.hpp"

// ========================================================================== //
// TilePatternRug Declaration
// ========================================================================== //

namespace dib::game {

/** Tile with resources from a rug-pattern texture **/
class TilePatternRug : public Tile
{
public:
  static constexpr u32 RUG_PATTERN_SUB_RESOURCE_COUNT = 16;

private:
  /** Paths for each sub-resource. These are the same as base path but with
   * appended 0-15 (eg. some_res_0) **/
  std::vector<ResourcePath> mSubPaths;

public:
  /** Construct from path to base resource **/
  explicit TilePatternRug(ResourcePath resourcePath);

  /** Returns correct sub-resource for location in world **/
  ResourcePath& GetResourcePath(World& world, u32 x, u32 y, u32 layer) override;
};

}

// ========================================================================== //
// TilePatternBlob Declaration
// ========================================================================== //
