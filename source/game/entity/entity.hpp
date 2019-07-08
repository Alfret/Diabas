#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"

// ========================================================================== //
// Entity Declaration
// ========================================================================== //

namespace dib::game {

/** Class that represents an entity in the world **/
class Entity
{
private:
public:
  /** Base virtual destructor **/
  virtual ~Entity() = default;

  /** Called to update the entity **/
  virtual void Update(f32 delta);
};

}