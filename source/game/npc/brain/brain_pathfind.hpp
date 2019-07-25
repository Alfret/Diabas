#ifndef BRAIN_PATHFIND_HPP_
#define BRAIN_PATHFIND_HPP_

#include "core/types.hpp"
#include "game/physics/units.hpp"
#include "game/ecs/entity_manager.hpp"

namespace dib::game {

class World;
class Moveable;

class BrainPathfind
{
public:
  BrainPathfind(Entity entity)
    : entity_(entity)
  {}

  /// @return If a path to target was found.
  bool Think(World& world, f32 delta, Moveable& me, Position target);

private:
  Entity entity_;
  std::vector<WorldPos> path_;
  f64 path_timer_;
};
}

#endif // BRAIN_PATHFIND_HPP_
