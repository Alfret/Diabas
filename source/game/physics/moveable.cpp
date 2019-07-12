#include "moveable.hpp"
#include <dutil/misc.hpp>
#include <dutil/stopwatch.hpp>
#include "game/world.hpp"
#include "game/physics/collision.hpp"
#include <microprofile/microprofile.h>

namespace dib::game {

bool MoveableIncrement::ToBytes(alflib::RawMemoryWriter& mw) const
{
  mw.Write(horizontal_velocity);
  mw.Write(vertical_velocity);
  mw.Write(jumping);
  mw.Write(position.x);
  mw.Write(position.y);
  return mw.Write(input);
}

MoveableIncrement MoveableIncrement::FromBytes(alflib::RawMemoryReader& mr)
{
  MoveableIncrement data{};
  data.horizontal_velocity = mr.Read<f32>();
  data.vertical_velocity = mr.Read<f32>();
  data.jumping = mr.Read<u8>();
  data.position.x = mr.Read<f32>();
  data.position.y = mr.Read<f32>();
  data.input = mr.Read<PlayerInput>();
  return data;
}

// ============================================================ //

MoveableIncrement Moveable::ToIncrement() const {
  MoveableIncrement m{};
  m.horizontal_velocity = horizontal_velocity;
  m.vertical_velocity = vertical_velocity;
  m.jumping = jumping;
  m.position = position;
  m.input = input;
  return m;
}

void Moveable::FromIncrement(const MoveableIncrement& m) {
  horizontal_velocity = m.horizontal_velocity;
  vertical_velocity = m.vertical_velocity;
  jumping = m.jumping;
  position = m.position;
  input = m.input;
}

bool Moveable::ToBytes(alflib::RawMemoryWriter& mw) const
{
  mw.Write(horizontal_velocity);
  mw.Write(vertical_velocity);
  mw.Write(velocity_input);
  mw.Write(velocity_max);
  mw.Write(velocity_jump);
  mw.Write(jumping);
  mw.Write(position.x);
  mw.Write(position.y);
  mw.Write(input);
  mw.Write(width);
  mw.Write(height);
  return mw.Write(collideable);
}

Moveable Moveable::FromBytes(alflib::RawMemoryReader& mr)
{
  Moveable data{};
  data.horizontal_velocity = mr.Read<f32>();
  data.vertical_velocity = mr.Read<f32>();
  data.velocity_input = mr.Read<f32>();
  data.velocity_max = mr.Read<f32>();
  data.velocity_jump = mr.Read<decltype(velocity_jump)>();
  data.jumping = mr.Read<decltype(jumping)>();
  data.position.x = mr.Read<f32>();
  data.position.y = mr.Read<f32>();
  data.input = mr.Read<PlayerInput>();
  data.width = mr.Read<f32>();
  data.height = mr.Read<f32>();
  data.collideable = mr.Read<Collideable>();
  return data;
}

// ============================================================ //

/**
 * @pre col_pos must be a colliding position for moveable.
 */
static CollisionInfo
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

static CollisionInfo
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
    if (col_info.VerticalCollision() && !col_info.HorizontalCollision()) {
      moveable.position.x = col_pos.x;
    } else if (col_info.HorizontalCollision() && !col_info.VerticalCollision()) {
      moveable.position.y = col_pos.y;
    }
  } else {
    moveable.position = positions.back();
  }

  return col_info;
}

/**
 * Based on the current moveable values, simulate a step of size @delta.
 */
static void
SimulateMoveable(const World& world,
           const f64 delta,
           Moveable& moveable)
{
  const auto& terrain = world.GetTerrain();
  const bool on_ground = OnGround(world, moveable);
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
      if (on_ground) {
        moveable.vertical_velocity = 0.0f;
      }
    }
  }
}

// ============================================================ //

void
SimulateMoveables(World& world, const f64 delta)
{
  MICROPROFILE_SCOPEI("player", "simulate moveables", MP_PURPLE);
  auto& registry = world.GetEntityManager().GetRegistry();

  if constexpr (kSide == Side::kClient) {
    auto view = registry.view<Moveable>();
    for (const auto entity : view) {
      MICROPROFILE_SCOPEI("player", "simulate moveable", MP_PURPLE1);
      Moveable& moveable = view.get(entity);
      UpdateMoveable(world, delta, moveable);
      SimulateMoveable(world, delta, moveable);
    }
  } else {
    auto view = registry.view<PlayerData, Moveable>();
    Packet packet{};
    world.GetNetwork().GetPacketHandler().BuildPacketHeader(
      packet, PacketHeaderStaticTypes::kPlayerIncrement);
    auto mw = packet.GetMemoryWriter();
    mw->Write(static_cast<u32>(view.size()));
    for (const auto entity : view) {
      MICROPROFILE_SCOPEI("player", "simulate moveable", MP_PURPLE1);
      Moveable& moveable = view.get<Moveable>(entity);
      UpdateMoveable(world, delta, moveable);
      SimulateMoveable(world, delta, moveable);
      mw->Write(moveable.ToIncrement());
      mw->Write(view.get<PlayerData>(entity).uuid);
    }
    mw.Finalize();

    dutil::FixedTimeUpdate(
        60, [&]() { world.GetNetwork().PacketBroadcast(packet); });
  }
}

void
UpdateMoveable(const World& world,
               const f64 delta,
               Moveable& moveable)
{
  const bool on_ground = OnGround(world, moveable);

  f32 h_vel = 0.0f;
  f32 v_vel = 0.0f;
  //f32 v_vel = 0.0f;
  if (moveable.input.Left()) {
    h_vel -= moveable.velocity_input;
  }
  if (moveable.input.Right()) {
    h_vel += moveable.velocity_input;
  }
  if (moveable.input.Jump()) {
    if (on_ground && moveable.vertical_velocity == 0) {
      ForceOnMoveable(moveable, 0.0f, moveable.velocity_jump);
    }
    moveable.jumping = 1;
  } else {
    moveable.jumping = 0;
  }

  // vertical velocity
  if (!on_ground) {
    if (moveable.vertical_velocity > 0.0f) {
      if (moveable.jumping) {
        moveable.vertical_velocity -= (v_vel + kStandardGravity * 3) * delta;
      } else {
        moveable.vertical_velocity = 0;
      }
    } else {
      // falling feels better if faster than rising
      moveable.vertical_velocity -= (v_vel + kStandardGravity * 5) * delta;
    }
  } else if (v_vel > 0.0f) {
    moveable.vertical_velocity = v_vel * delta;
  }
  moveable.jumping = 0;

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
}

void
ForceOnMoveable(Moveable& moveable,
              const f32 horizontal_force,
              const f32 vertical_force)
{
  moveable.horizontal_velocity += horizontal_force;
  moveable.vertical_velocity += vertical_force;
}

void
ApplyMoveableIncrement(Moveable& moveable, const MoveableIncrement increment,
                       const bool is_our_moveable)
{
  if constexpr (kSide == Side::kServer) {
      AlfAssert(false, "Server should not apply moveable increments");
    }

    if (!is_our_moveable) {
      moveable.input = increment.input;
    }

  static constexpr f32 kMaxDiff = kTileInMeters * 5.0f;
  if (std::abs(moveable.position.x - increment.position.x) > kMaxDiff) {
    moveable.position.x = increment.position.x;
    moveable.horizontal_velocity = increment.horizontal_velocity;
  }
  if (std::abs(moveable.position.y - increment.position.y) > kMaxDiff) {
    moveable.position.y = increment.position.y;
    moveable.vertical_velocity = increment.vertical_velocity;
  }
}
}
