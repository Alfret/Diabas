#ifndef BRAIN_FOLLOW_HPP_
#define BRAIN_FOLLOW_HPP_

#include "core/types.hpp"
#include "game/physics/units.hpp"
#include "game/ecs/entity_manager.hpp"

namespace dib::game {

class World;
class Moveable;

class BrainFollow
{
public:
  BrainFollow(Entity entity)
    : entity_(entity)
  {}

  void Think(World& world, f32 delta, Moveable& me, Position target);

private:
  Entity entity_;
};
}

#endif // BRAIN_FOLLOW_HPP_
