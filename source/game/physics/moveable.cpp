#include "moveable.hpp"
#include <dutil/misc.hpp>
#include "game/world.hpp"
#include "game/physics/collision.hpp"

namespace dib::game {

/**
 * @pre col_pos must be a colliding position for moveable.
 */
CollisionInfo
MoveSubTileOnCollision(const World& world, Moveable& moveable,
                       Position col_pos)
{
  // @PERF this function has lots to gain

  std::vector<Position> positions;
  GeneratePositions(moveable.position, col_pos, positions, kPixelInMeter);

  // make ok_pos and col_pos distance differ in only one kPixelInMeter
  bool colliding = false;
  Position ok_pos = moveable.position;
  for (u32 i = 0; i < positions.size(); i++) {
    if (CollidesOnPosition(world, moveable.collideable, positions[i])) {
      colliding = true;
      col_pos = positions[i];
      if (i > 0) {
        ok_pos = positions[i - 1];
      }
      break;
    }
  }
  AlfAssert(colliding, "did not collide - but it is a precondition");
  moveable.position = ok_pos;

  // what direction did we collide in?
  CollisionInfo col_info{};
  const Position left{ok_pos.x - kPixelInMeter, ok_pos.y};
  const Position right{ok_pos.x + kPixelInMeter, ok_pos.y};
  const Position up{ok_pos.x, ok_pos.y + kPixelInMeter};
  const Position down{ ok_pos.x, ok_pos.y - kPixelInMeter };
  if (CollidesOnPosition(world, moveable.collideable, left)) {
    col_info.x -= 1.0f;
  }
  if (CollidesOnPosition(world, moveable.collideable, right)) {
    col_info.x += 1.0f;
  }
  if (CollidesOnPosition(world, moveable.collideable, up)) {
    col_info.y += 1.0f;
  }
  if (CollidesOnPosition(world, moveable.collideable, down)) {
    col_info.y -= 1.0f;
  }

  return col_info;
}

CollisionInfo
MoveMoveable(const World& world,
             Moveable& moveable,
             const Position target)
{
  CollisionInfo col_info{};

  std::vector<Position> positions;
  GeneratePositions(moveable.position, target, positions);
  AlfAssert(positions.size() > 0, "Failed to generate positions");

  bool colliding = false;
  Position ok_pos = moveable.position;
  Position col_pos;
  for (u32 i = 0; i < positions.size(); i++) {
    if (CollidesOnPosition(world, moveable.collideable, positions[i])) {
      colliding = true;
      col_pos = positions[i];
      if (i > 0) {
        ok_pos = positions[i - 1];
      }
      break;
    }
  }

  if (colliding) {
    moveable.position = ok_pos;
    col_info = MoveSubTileOnCollision(world, moveable, col_pos);
  } else {
    moveable.position = positions.back();
  }

  return col_info;
}

void
UpdateMoveable(const World& world,
           const f64 delta,
           Moveable& moveable,
           const f32 h_vel,
           const f32 v_vel)
{
  const auto& terrain = world.GetTerrain();
  const bool on_ground = OnGround(world, moveable);

  // vertical velocity
  if (!on_ground) {
    if (moveable.vertical_velocity > 0.0f) {
      moveable.vertical_velocity -= (v_vel + kStandardGravity * 3) * delta;
    } else {
      // falling feels better if faster than rising
      moveable.vertical_velocity -= (v_vel + kStandardGravity * 5) * delta;
    }
  } else if (v_vel > 0.0f) {
    moveable.vertical_velocity = v_vel * delta;
  }

  // horizontal velocity
  if (std::abs(h_vel) > 0.0f) {
    moveable.horizontal_velocity = dutil::Clamp(
      moveable.horizontal_velocity + h_vel * static_cast<f32>(delta) -
        moveable.horizontal_velocity * static_cast<f32>(delta),
      -moveable.velocity_max,
      moveable.velocity_max);
  } else if (on_ground ||
             (on_ground && std::abs(std::copysignf(1.0f, moveable.horizontal_velocity) +
                                    std::copysignf(1.0f, h_vel)) < 1.0f)) {
    moveable.horizontal_velocity = 0.0f;
  }

  Position new_position = moveable.position;

  // calculate x position
  bool moved_x = false;
  if (!alflib::FloatEqual(moveable.horizontal_velocity, 0.0f)) {
    new_position.x =
      dutil::Clamp(static_cast<f32>(moveable.position.x +
                                    moveable.horizontal_velocity * delta),
                   0.0f,
                   static_cast<f32>(terrain.GetWidth() - 1));
    if (!alflib::FloatEqual(new_position.x, moveable.position.x)) {
       moved_x = true;
    }
  }

  // calculate y position
  bool moved_y = false;
  if (!alflib::FloatEqual(moveable.vertical_velocity, 0.0f)) {
    new_position.y = dutil::Clamp(
      static_cast<f32>(moveable.position.y + moveable.vertical_velocity *
delta), 0.0f, static_cast<f32>(terrain.GetHeight()-1));
    if (!alflib::FloatEqual(new_position.y, moveable.position.y)) {
      moved_y = true;
    }
  }

  if (moved_x || moved_y) {
    const CollisionInfo col_info = MoveMoveable(world, moveable, new_position);

    // x
    if (col_info.HorizontalCollision()) {
      moveable.horizontal_velocity = 0.0f;
    }

    // y
    if (col_info.VerticalCollision()) {
      moveable.vertical_velocity = 0.0f;
    }
  }
}

void
ForceOnMoveable(Moveable& moveable,
              const f32 horizontal_force,
              const f32 vertical_force)
{
  moveable.horizontal_velocity += horizontal_force;
  moveable.vertical_velocity += vertical_force;
}


}
