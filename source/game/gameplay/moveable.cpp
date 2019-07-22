#include "moveable.hpp"
#include <dutil/misc.hpp>
#include <dutil/stopwatch.hpp>
#include "game/world.hpp"
#include "game/physics/collision.hpp"
#include <microprofile/microprofile.h>

namespace dib::game {

bool
MoveableIncrement::ToBytes(alflib::RawMemoryWriter& mw) const
{
  mw.Write(horizontal_velocity);
  mw.Write(vertical_velocity);
  mw.Write(force.h_vel);
  mw.Write(force.v_vel);
  mw.Write(force.duration_s);
  mw.Write(jumping);
  mw.Write(position.x);
  mw.Write(position.y);
  return mw.Write(input);
}

MoveableIncrement
MoveableIncrement::FromBytes(alflib::RawMemoryReader& mr)
{
  MoveableIncrement data{};
  data.horizontal_velocity = mr.Read<f32>();
  data.vertical_velocity = mr.Read<f32>();
  data.force.h_vel = mr.Read<Velocity>();
  data.force.v_vel = mr.Read<Velocity>();
  data.force.duration_s = mr.Read<f32>();
  data.jumping = mr.Read<u8>();
  data.position.x = mr.Read<f32>();
  data.position.y = mr.Read<f32>();
  data.input = mr.Read<PlayerInput>();
  return data;
}

// ============================================================ //

MoveableIncrement
Moveable::ToIncrement() const
{
  MoveableIncrement m{};
  m.horizontal_velocity = horizontal_velocity;
  m.vertical_velocity = vertical_velocity;
  m.force = force;
  m.jumping = jumping;
  m.position = position;
  m.input = input;
  return m;
}

void
Moveable::FromIncrement(const MoveableIncrement& m)
{
  horizontal_velocity = m.horizontal_velocity;
  vertical_velocity = m.vertical_velocity;
  force = m.force;
  jumping = m.jumping;
  position = m.position;
  input = m.input;
}

bool
Moveable::ToBytes(alflib::RawMemoryWriter& mw) const
{
  mw.Write(horizontal_velocity);
  mw.Write(vertical_velocity);
  mw.Write(force.h_vel);
  mw.Write(force.v_vel);
  mw.Write(force.duration_s);
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

Moveable
Moveable::FromBytes(alflib::RawMemoryReader& mr)
{
  Moveable data{};
  data.horizontal_velocity = mr.Read<decltype(horizontal_velocity)>();
  data.vertical_velocity = mr.Read<decltype(vertical_velocity)>();
  data.force.h_vel = mr.Read<decltype(force.h_vel)>();
  data.force.v_vel = mr.Read<decltype(force.v_vel)>();
  data.force.duration_s = mr.Read<decltype(force.duration_s)>();
  data.velocity_input = mr.Read<decltype(velocity_input)>();
  data.velocity_max = mr.Read<decltype(velocity_max)>();
  data.velocity_jump = mr.Read<decltype(velocity_jump)>();
  data.jumping = mr.Read<decltype(jumping)>();
  data.position.x = mr.Read<decltype(position.x)>();
  data.position.y = mr.Read<decltype(position.y)>();
  data.input = mr.Read<decltype(input)>();
  data.width = mr.Read<decltype(width)>();
  data.height = mr.Read<decltype(height)>();
  data.collideable = mr.Read<decltype(collideable)>();
  return data;
}

// ============================================================ //

/**
 * Move the moveable in increments of a single pixel until we collide.
 * Then figure out if we collided left, right, up or down.
 *
 * @pre col_pos must be a colliding position for moveable.
 */
static CollisionInfo
MoveSubTileOnCollision(const World& world, Moveable& moveable, Position col_pos)
{
  std::vector<Position> positions;
  GeneratePositions(moveable.position, col_pos, positions, kPixelInMeter);

  // make ok_pos and col_pos distance differ in only one kPixelInMeter
  bool colliding = false;
  Position ok_pos = moveable.position;
  std::optional<WorldPos> maybe_pos;
  for (u32 i = 0; i < positions.size(); i++) {
    maybe_pos = CollidesOnPosition(world, moveable.collideable, positions[i]);
    if (maybe_pos &&
        world.GetTerrain().GetTile(*maybe_pos)->GetCollisionIsSolid()) {
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
  const Position horizontal { col_pos.x, ok_pos.y };
  const Position vertical { ok_pos.x, col_pos.y};

  if (maybe_pos = CollidesOnPosition(world, moveable.collideable, horizontal);
      maybe_pos &&
      world.GetTerrain().GetTile(*maybe_pos)->GetCollisionIsSolid()) {
    col_info.x += col_pos.x - ok_pos.x;
  }
  if (maybe_pos = CollidesOnPosition(world, moveable.collideable, vertical);
      maybe_pos &&
      world.GetTerrain().GetTile(*maybe_pos)->GetCollisionIsSolid()) {
    col_info.y += col_pos.y - ok_pos.y;
  }

  return col_info;
}

static CollisionInfo
MoveMoveable(World& world,
             Moveable& moveable,
             const Position target,
             const entt::entity entity)
{
  CollisionInfo col_info{};

  std::vector<Position> positions;
  GeneratePositions(moveable.position, target, positions);
  AlfAssert(positions.size() > 0, "Failed to generate positions");

  bool colliding = false;
  Position ok_pos = moveable.position;
  Position col_pos;
  WorldPos tile_col_pos;
  Tile* tile_col_tile = nullptr;
  std::optional<WorldPos> maybe_tile_col_pos;
  for (u32 i = 0; i < positions.size(); i++) {
    maybe_tile_col_pos =
      CollidesOnPosition(world, moveable.collideable, positions[i]);
    if (maybe_tile_col_pos) {
      colliding = true;
      col_pos = positions[i];
      if (i > 0) {
        ok_pos = positions[i - 1];
      }
      tile_col_pos = *maybe_tile_col_pos;
      tile_col_tile = world.GetTerrain().GetTile(tile_col_pos);
      tile_col_tile->OnCollision(world, tile_col_pos, entity);
      if (tile_col_tile->GetCollisionIsSolid()) {
        break;
      }
    }
  }

  if (colliding) {
    if (tile_col_tile->GetCollisionIsSolid()) {
      // Move as close as possible to colliding tile
      moveable.position = ok_pos;
      col_info = MoveSubTileOnCollision(world, moveable, col_pos);
      if (col_info.VerticalCollision() && !col_info.HorizontalCollision()) {
        moveable.position.x = col_pos.x;
      } else if (col_info.HorizontalCollision() &&
                 !col_info.VerticalCollision()) {
        moveable.position.y = col_pos.y;
      }
    } else {
      // Move to target, since not solid
      moveable.position = target;
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
SimulateMoveable(World& world,
                 const f64 delta,
                 Moveable& moveable,
                 const entt::entity entity)
{
  const auto& terrain = world.GetTerrain();
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
    new_position.y =
      dutil::Clamp(static_cast<f32>(moveable.position.y +
                                    moveable.vertical_velocity * delta),
                   0.0f,
                   static_cast<f32>(terrain.GetHeight() - 1));
    if (!alflib::FloatEqual(new_position.y, moveable.position.y)) {
      moved_y = true;
    }
  }

  if (moved_x || moved_y) {
    const CollisionInfo col_info =
      MoveMoveable(world, moveable, new_position, entity);

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

static void
UpdateMoveable(World& world,
               const f64 delta,
               Moveable& moveable,
               const entt::entity entity)
{
  const bool on_ground = OnGround(world, moveable);

  f32 h_vel = 0.0f;
  f32 v_vel = 0.0f;

  if (moveable.input.Left()) {
    h_vel -= moveable.velocity_input;
  }
  if (moveable.input.Right()) {
    h_vel += moveable.velocity_input;
  }
  if (moveable.input.Jump()) {
    if (on_ground && alflib::FloatEqual(moveable.vertical_velocity, 0.0f)) {
      moveable.vertical_velocity += moveable.velocity_jump;
    }
    moveable.jumping = 1;
  } else {
    moveable.jumping = 0;
  }

  // vertical velocity
  if (!on_ground) {
    if (moveable.vertical_velocity > 0.0f) {
      if (moveable.jumping) {
        moveable.vertical_velocity += (v_vel - (kStandardGravity * 3)) * delta;
      } else {
        moveable.vertical_velocity = v_vel;
      }
    } else {
      // falling feels better if faster than rising
      moveable.vertical_velocity += (v_vel - (kStandardGravity * 5)) * delta;
    }
  } else if (v_vel > 0.0f) {
    moveable.vertical_velocity = v_vel;
  }
  moveable.jumping = 0;

  // horizontal velocity
  if ((on_ground && std::abs(moveable.horizontal_velocity) > 0.1f &&
       std::abs(std::copysignf(1.0f, moveable.horizontal_velocity) +
                std::copysignf(1.0f, h_vel)) < 1.0f)) {
    // instantly stop if changing direction while moving.
    moveable.horizontal_velocity = 0.0f;
  } else if (std::abs(h_vel) > 0.0f) {
    moveable.horizontal_velocity =
      dutil::Clamp(moveable.horizontal_velocity + h_vel,
                   -moveable.velocity_max,
                   moveable.velocity_max);
  } else if (on_ground) {
    // instantly stop if no input
    moveable.horizontal_velocity = 0.0f;
  }

  if (moveable.force.duration_s > static_cast<f32>(delta)) {
    moveable.horizontal_velocity += moveable.force.h_vel;
    moveable.vertical_velocity += moveable.force.v_vel;
    moveable.force.duration_s -= delta;
  }

  SimulateMoveable(world, delta, moveable, entity);
}

// ============================================================ //

void
UpdateMoveables(World& world, const f64 delta)
{
  MICROPROFILE_SCOPEI("player", "simulate moveables", MP_PURPLE);
  auto& registry = world.GetEntityManager().GetRegistry();

  if constexpr (kSide == Side::kClient) {
    auto view = registry.view<Moveable>();
    for (const auto entity : view) {
      MICROPROFILE_SCOPEI("player", "simulate moveable", MP_PURPLE1);
      UpdateMoveable(world, delta, view.get(entity), entity);
    }
    } else /* if (kSide == Side==kServer) */ {
      auto view = registry.view<Moveable>();
      for (const auto entity : view) {
        // We don't update players on server.
        if (!registry.has<PlayerData>(entity)) {
          MICROPROFILE_SCOPEI("player", "simulate moveable", MP_PURPLE1);
          UpdateMoveable(world, delta, view.get(entity), entity);
        }
      }
  }
}

void
ForceOnMoveable(Moveable& moveable, const Force force)
{
  moveable.force = force;
}

void
ApplyMoveableIncrement(Moveable& moveable,
                       const MoveableIncrement increment,
                       const bool is_our_moveable)
{
  if constexpr (kSide == Side::kServer) {
    AlfAssert(false, "Server should not apply moveable increments");
  }

  if (!is_our_moveable) {
    moveable.FromIncrement(increment);
  }
}
}
