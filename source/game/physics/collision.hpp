#ifndef COLLISION_HPP_
#define COLLISION_HPP_

#include "core/types.hpp"
#include "game/physics/units.hpp"
#include "game/physics/collideable.hpp"
#include <vector>

namespace dib::game {

struct MoveableEntity;

bool
CollidesOnPosition(const World& world,
                   const Collideable& collideable,
                   Position position);

// ============================================================ //
/**
 * Is the point touching the ground, aka standing, aka not falling.
 */
bool
OnGround(const World& world, const MoveableEntity& entity);

}

#endif // COLLISION_HPP_
