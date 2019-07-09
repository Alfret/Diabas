#ifndef COLLISION_HPP_
#define COLLISION_HPP_

#include "core/types.hpp"
#include "game/physics/units.hpp"
#include "game/physics/collideable.hpp"
#include "game/physics/moveable.hpp"
#include <vector>

namespace dib::game {

struct MoveableEntity;

/**
 * Is the @collidable colliding with any tiles when on @position?
 */
bool
CollidesOnPosition(const World& world,
                   const Collideable& collideable,
                   Position position);

/**
 * Is the moveable touching the ground, aka standing, aka not falling.
 */
bool
OnGround(const World& world, const Moveable& moveable);

/**
 * Make a collection of positions on the range [@a, @b], in steps of
 * kTileInMeters. Useful for generating a set of possible collisions
 * when moving from @a to @b.
 */
void
GeneratePositions(const Position a,
                  const Position b,
                  std::vector<Position>& positions);
}

#endif // COLLISION_HPP_
