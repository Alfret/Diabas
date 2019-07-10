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
  f32 horizontal_velocity;

  /**
   * negative -> down
   * positive -> up
   */
  f32 vertical_velocity;

  f32 velocity_input;
  f32 velocity_max;
  f32 velocity_jump;
  bool jumping;

  /**
   * position is specified in meters
   * position origin is at middle x, bottom y, (right at the toes), of the
   * character
   */
  game::Position position;

  /**
   * Visual width and height
   */
  f32 width;
  f32 height;

  /**
   * Physical footprint of the moveable.
   */
  Collideable collideable;
};

// ============================================================ //
// Functions
// ============================================================ //

/**
 * Simulate what all the moveables will do in the given time step.
 */
void
SimulateMoveables(World& world, f64 delta);

/**
 * Prepare the moveable with fresh input.
 */
void
UpdateMoveable(const World& world,
               f64 delta,
               Moveable& moveable,
               f32 h_vel,
               f32 v_vel);

/**
 * Apply a force to a moveable. This means, instantly, modifying the entities
 * velocity.
 *
 * Note, this will not move the moveable, good practice would be to apply the
 * force, then moving the moveable.
 */
void ForceOnMoveable(Moveable& moveable,
                     f32 horizontal_force,
                     f32 vertical_force);

inline Moveable
MoveableMakeDefault()
{
  Moveable m{};
  m.width = game::kTileInMeters * 1.5f;
  m.height = game::kTileInMeters * 2.9f;
  m.velocity_input = 20.0f;
  m.velocity_max = 10.0f;
  m.velocity_jump = 14.0f;
  m.jumping = true;
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
