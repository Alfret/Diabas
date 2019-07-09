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
 * Is the point inside, or on, tile?
 */
bool
PointInsideTile(Position point, WorldPos tile);

/**
 * Is the given point currently colliding with the tile at position tile?
 */
bool
PointCollideWithTile(const World& world, Position point, WorldPos tile);
bool
PointCollideWithTile(const World& world, Position point);

/**
 * Is the given point currently colliding with anything?
 */
bool
PointColliding(const World& world, Position point);

/**
 * Is the point currently colliding?
 */
bool
CheckCollision(const World& world, Position point, Collideable collision);

/**
 * Is the point touching the ground, aka standing, aka not falling.
 */
bool
OnGround(const World& world, const MoveableEntity& entity);

struct CollisionResult
{
  bool x_collision;
  bool y_collision;
};

CollisionResult
TryMoveEntity(const World& world,
              MoveableEntity& entity,
              const Position target);

}

#endif // COLLISION_HPP_
