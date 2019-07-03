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

  /** Base destructor **/
  virtual ~TileEntity() = default;

  /** Called to update the tile entity **/
  virtual void Update(){};

  /** Called when the tile entity should be loaded from a value-store **/
  virtual void OnLoad(ValueStore& valueStore){};

  /** Called when the tile entity should be saved to a value-store **/
  virtual void OnStore(ValueStore& valueStore){};
};

}