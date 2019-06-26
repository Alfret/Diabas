#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include "game/resource.hpp"

// ========================================================================== //
// Tile Declaration
// ========================================================================== //

namespace dib::game {

/** Tile base class **/
class Tile
{
protected:
  /** Path to the resource **/
  ResourcePath mResourcePath;

public:
  Tile(const ResourcePath& resourcePath);

  /** Base destructor **/
  virtual ~Tile() = default;

  /** Returns the resource path of the tile **/
  virtual ResourcePath& GetResourcePath(u32 x, u32 y) { return mResourcePath; }
};

}