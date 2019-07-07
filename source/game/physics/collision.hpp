#ifndef COLLISION_HPP_
#define COLLISION_HPP_

#include "core/types.hpp"
#include "game/physics/units.hpp"

namespace dib::game {

/** Enumeration of collision types. This determines how entities collide with
 * the tile **/
enum class CollisionType
{
  /** Full collision **/
  kFull,
  /** No collision **/
  kNone,
  /** Stairs collision **/
  kStairs,
  /** Custom collision. 'Tile::GetAABB()' is called to determine collision **/
};

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

/**
 * Is the given point currently colliding with anything?
 */
bool
PointColliding(const World& world, Position point);

/**
 * Is the point touching the ground, aka standing, aka not falling.
 */
bool
OnGround(const World& world, Position point);

/**
 * Attempt to move source point to target point. If a tile, with collision,
 * is in the way, we will stop just before that tile.
 */
bool
MovePointCollideable(const World& world, Position& point, Position target);

/**
 * Move your entity, by inputting vertical and horizontal acceleration.
 * @param h_acc Delta should already be applied.
 * @param v_acc Delta should already be applied.
 */
void
MoveEntity(const World& world,
           f64 delta,
           MoveableEntity* entity,
           Acceleration h_acc,
           Acceleration v_acc);

/**
 * Apply a force to an entity. This means, instantly, modifying the entities
 * velocity.
 *
 * Note, this will not move the entity, good practice would be to apply the
 * force, then moving the entity.
 */
void
ForceOnEntity(MoveableEntity* entity, f32 horizontal_force, f32 vertical_force);
}

#endif // COLLISION_HPP_
