#include "moveable.hpp"
#include <dutil/misc.hpp>
#include "game/world.hpp"

namespace dib::game {

/*
void
MoveMoveable(const World& world,
           const f64 delta,
           Moveable& moveable,
           const Acceleration h_acc,
           const Acceleration v_acc)
{
  const auto& terrain = world.GetTerrain();
  const Position old_position = moveable.position;
  const bool on_ground = OnGround(world, moveable);

  // calculate acceleration
  constexpr float kExtreme = kStandardGravity * kMaxAccelerationModifier;
  if (!alflib::FloatEqual(h_acc, 0.0f)) {
    const float max_acceleration =
      std::min(kExtreme, moveable.horizontal_max_acceleration);
    moveable.horizontal_acceleration =
      dutil::Clamp(moveable.horizontal_acceleration + h_acc,
                   -max_acceleration,
                   max_acceleration);
  } else {
    // cancel out our acceleration
    constexpr f32 kAdjustmentFactor = 50.0f;
    if (moveable.horizontal_acceleration < 0.0f) {
      moveable.horizontal_acceleration += kAdjustmentFactor * delta;
      if (moveable.horizontal_acceleration > 0.0f) {
        // cancel out over adjustments
        moveable.horizontal_acceleration = 0.0f;
      }
    } else if (moveable.horizontal_acceleration > 0.0f) {
      moveable.horizontal_acceleration -= kAdjustmentFactor * delta;
      if (moveable.horizontal_acceleration < 0.0f) {
        // cancel out over adjustments
        moveable.horizontal_acceleration = 0.0f;
      }
    }
  }

  if (!alflib::FloatEqual(v_acc, 0.0f)) {
    moveable.vertical_acceleration =
      dutil::Clamp(moveable.vertical_acceleration + v_acc, -kExtreme, kExtreme);
  }

  Acceleration v_acc_gravity = moveable.vertical_acceleration;
  if (!on_ground) {
    v_acc_gravity -= kStandardGravity * 4;
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
    moved_x = true;
    new_position.x = dutil::Clamp(
      static_cast<f32>(moveable.position.x + moveable.horizontal_velocity *
delta), 0.0f, static_cast<f32>(terrain.GetWidth() - 1));
  }

  // calculate y position
  bool moved_y = false;
  if ((!on_ground || moveable.vertical_velocity > 0.0f) &&
      !alflib::FloatEqual(moveable.vertical_velocity, 0.0f)) {
    moved_y = true;
    new_position.y = dutil::Clamp(
      static_cast<f32>(moveable.position.y + moveable.vertical_velocity *
delta), 0.0f, static_cast<f32>(terrain.GetHeight()-1));
  }

  if (moved_x || moved_y) {

    const CollisionResult result = TryMoveMoveable(world, moveable,
new_position);

    // x
    if (result.x_collision) {
      moveable.horizontal_acceleration = 0.0f;
      moveable.horizontal_velocity = 0.0f;
    } else if (on_ground) {
      // apply friction
      constexpr f32 kFrictionConstant = 5.0f;
      if (moveable.horizontal_velocity < 0.0f) {
        moveable.horizontal_velocity -=
          (moveable.horizontal_velocity - kFrictionConstant) * moveable.friction
* delta; if (moveable.horizontal_velocity > 0.0f) {
          // cancel out over adjustments
          moveable.horizontal_velocity = 0.0f;
        }
      } else if (moveable.horizontal_velocity > 0.0f) {
        moveable.horizontal_velocity -=
          (moveable.horizontal_velocity + kFrictionConstant) * moveable.friction
* delta; if (moveable.horizontal_velocity < 0.0f) {
          // cancel out over adjustments
          moveable.horizontal_velocity = 0.0f;
        }
      }
    }

    // y
    if (result.y_collision) {
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
*/

}
