#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/world.hpp"

// ========================================================================== //
// TileEntity Declaration
// ========================================================================== //

namespace dib::game {

/** Class that represents an entity connected to a single tile in the world.
 * This is used to implement special behaviour for tiles **/
class TileEntity
{
public:
  /** Construct tile entity at position **/
  TileEntity(WorldPos pos);

  /** Called to update the tile entity **/
  virtual void Update(){};

  virtual void OnLoad(){};

  virtual void OnStore(){};
};

}