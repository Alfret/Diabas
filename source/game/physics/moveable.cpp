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
           const Acceleration h_acc,
           const Acceleration v_acc)
{
  const auto& terrain = world.GetTerrain();
  const bool on_ground = OnGround(world, moveable);

  // calculate acceleration
  constexpr float kExtreme = kStandardGravity * kMaxAccelerationModifier;
  if (!alflib::FloatEqual(h_acc, 0.0f)) {
    const float max_acceleration =
        std::min(kExtreme, moveable.horizontal_max_acceleration);

    f32 mod = dutil::Clamp(h_acc, -max_acceleration, max_acceleration) *
              static_cast<f32>(delta);
    if (std::abs(std::copysignf(1.0f, moveable.horizontal_velocity) +
                 std::copysign(1.0f, moveable.horizontal_acceleration)) <
        1.0f) {
      mod *= 3; }

    moveable.horizontal_acceleration = moveable.horizontal_acceleration + mod;
  } else {
    moveable.horizontal_acceleration = 0.0f;
  }

  if (!alflib::FloatEqual(v_acc, 0.0f)) {
    moveable.vertical_acceleration =
        dutil::Clamp(moveable.vertical_acceleration + v_acc * static_cast<f32>(delta), -kExtreme, kExtreme);
  } else {
    moveable.vertical_acceleration = 0.0f;
  }

  Acceleration v_acc_gravity = moveable.vertical_acceleration;
  if (!on_ground) {
    if (moveable.vertical_velocity > 0.0f) {
      v_acc_gravity -= kStandardGravity * 3;
    } else {
      // falling feels better if faster than rising
      v_acc_gravity -= kStandardGravity * 5;
    }
  }

  // calculate velocity based on acceleration
  // TODO maby remove these checks? always add?
  bool has_h_acc = false;
  if (!alflib::FloatEqual(moveable.horizontal_acceleration, 0.0f)) {
    has_h_acc = true;
    moveable.horizontal_velocity += moveable.horizontal_acceleration * delta;
  }
  if (!alflib::FloatEqual(v_acc_gravity, 0.0f)) {
    moveable.vertical_velocity += v_acc_gravity * delta;
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
      moveable.horizontal_acceleration = 0.0f;
      moveable.horizontal_velocity = 0.0f;
    } else if (on_ground && alflib::FloatEqual(moveable.horizontal_acceleration, 0.0f)) {
      // apply friction
      const f32 clamp = moveable.horizontal_velocity > 0.0f ? 10.0f : -10.0f;
      moveable.horizontal_velocity -= ((moveable.horizontal_velocity + clamp) * moveable.friction * delta);
      if (std::abs(moveable.horizontal_velocity) < kPixelInMeter / 10.0f) {
        moveable.horizontal_velocity = 0.0f;
      }
      // constexpr f32 kFrictionConstant = 5.0f;
//       if (moveable.horizontal_velocity < 0.0f) {
//         moveable.horizontal_velocity -=
//           (moveable.horizontal_velocity) * moveable.friction * delta;
//         if (moveable.horizontal_velocity > 0.0f) {
//           // cancel out over adjustments
//           moveable.horizontal_velocity = 0.0f;
//         }
//       } else if (moveable.horizontal_velocity > 0.0f) {
//         moveable.horizontal_velocity -=
//           (moveable.horizontal_velocity + kFrictionConstant) * moveable.friction
// * delta; if (moveable.horizontal_velocity < 0.0f) {
//           // cancel out over adjustments
//           moveable.horizontal_velocity = 0.0f;
//         }
//       }
    }

    // y
    if (col_info.VerticalCollision()) {
      moveable.vertical_acceleration = 0.0f;
      moveable.vertical_velocity = 0.0f;

    } else {
      // apply drag (vertical friction)
      constexpr f32 kFrictionConstant = 5.0f;
      if (moveable.vertical_velocity < -kStandardGravity) {
        moveable.vertical_velocity -=
          (moveable.vertical_velocity - kFrictionConstant) * moveable.drag *
delta; if (moveable.vertical_velocity > 0.0f) {
          // cancel out over adjustments
          moveable.vertical_velocity = 0.0f;
        }
      } else if (moveable.vertical_velocity > kStandardGravity) {
        moveable.vertical_velocity -=
          (moveable.vertical_velocity + kFrictionConstant) * moveable.drag *
delta; if (moveable.vertical_velocity < 0.0f) {
          // cancel out over adjustments
          moveable.vertical_velocity = 0.0f;
        }
      }
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
