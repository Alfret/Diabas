#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <vector>

#include "core/types.hpp"
#include "core/macros.hpp"
#include "game/resource.hpp"

// ========================================================================== //
// Tile Declaration
// ========================================================================== //

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(World);

/** Tile base class **/
class Tile
{
  friend class TileManager;

public:
  /** Tile ID type **/
  using ID = u16;

protected:
  /** ID of the tile. Set by the TileManager once the tile has been
   * registered **/
  ID mID;

  /** Paths to the resources **/
  std::vector<ResourcePath> mResourcePaths;

public:
  /** Construct tile by specifying the resource path. Override the class to
   * support using multiple resources per tile **/
  explicit Tile(const ResourcePath& resourcePath);

  /** Returns the ID of the tile **/
  ID GetID() const { return mID; }

  /** Base destructor **/
  virtual ~Tile() = default;

  /** Returns the resource path of the tile **/
  virtual ResourcePath& GetResourcePath(World& world, u32 x, u32 y, u32 layer);

  /** Returns the list of paths to all resources used by this tile. For them to
   * be usable when returned from the 'Tile::GetResourcePath()' they must have
   * been returned from this call first **/
  virtual std::vector<ResourcePath>& GetResourcePaths();
};

}
