#ifndef MOVEABLE_HPP_
#define MOVEABLE_HPP_

#include "game/physics/units.hpp"
#include "core/types.hpp"
#include "game/physics/collideable.hpp"

namespace dib::game {
struct Moveable
{
  /**
   * negative -> left
   * positive -> right
   */
  game::Velocity horizontal_velocity;

  /**
   * negative -> down
   * positive -> up
   */
  game::Velocity vertical_velocity;

  /**
   * position is specified in meters
   * position origin is at middle x, bottom y, (right at the toes), of the
   * character
   */
  game::Position position;

  game::Acceleration horizontal_acceleration;
  game::Acceleration vertical_acceleration;

  f32 horizontal_acceleration_modifier;
  f32 horizontal_max_acceleration;

  /**
   * Visual width and height
   */
  f32 width;
  f32 height;

  /**
   * Physical footprint of the moveable.
   */
  Collideable collideable;

  /**
   * Friction is applied when in contact with ground.
   * How strong is the friction? Must be in range [0.0, 1.0].
   */
  f32 friction;

  /**
   * Drag is applied when falling. Must be in range [0.0, 1.0].
   */
  f32 drag;
};

// ============================================================ //
// Functions
// ============================================================ //

/**
 * Move your moveable, by inputting vertical and horizontal acceleration.
 * @param h_acc Delta should already be applied.
 * @param v_acc Delta should already be applied.
 */
void
UpdateMoveable(const World& world,
               f64 delta,
               Moveable& moveable,
               Acceleration h_acc,
               Acceleration v_acc);

/**
 * Apply a force to a moveable. This means, instantly, modifying the entities
 * velocity.
 *
 * Note, this will not move the moveable, good practice would be to apply the
 * force, then moving the moveable.
 */
void
ForceOnMoveable(Moveable& moveable, f32 horizontal_force, f32 vertical_force);

inline Moveable
MoveableMakeDefault()
{
  Moveable m{};
  m.friction = 0.7f;
  m.drag = 0.7f;
  m.width = game::kTileInMeters * 1.5f;
  m.height = game::kTileInMeters * 2.9f;
  m.horizontal_acceleration_modifier = 100.0f;
  m.horizontal_max_acceleration = 12.0f;
  m.position.x = game::TileToMeter(10);
  m.position.y = game::TileToMeter(20);

  CollideableRect c{};
  c.type = game::CollisionType::kRect;
  c.rect = CollisionRectNoOffset{ game::kTileInMeters * 1.4f,
                                  game::kTileInMeters * 2.5f };
  std::memcpy(&m.collideable, &c, sizeof(c));

  return m;
}
}

#endif // MOVEABLE_HPP_
