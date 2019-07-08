#include "collision.hpp"
#include "game/world.hpp"
#include <dutil/misc.hpp>

namespace dib::game {

bool
PointInsideTile(const Position point, const WorldPos tile)
{
  const f32 px = MeterToPixel(point.x);
  const f32 py = MeterToPixel(point.y);
  const f32 tx = TileToPixel(tile.X());
  const f32 ty = TileToPixel(tile.Y());
  constexpr f32 twidth = kTileInPixels;
  constexpr f32 theight = kTileInPixels;
  return (tx <= px && px <= tx + twidth && ty <= py && py <= ty + theight);
}

bool
PointCollideWithTile(const World& world,
                     const Position point,
                     const WorldPos tile_pos)
{
  auto& terrain = world.GetTerrain();
  const Tile* tile = terrain.GetTile(tile_pos);
  if (auto collision_type = tile->GetCollision(world, tile_pos);
      collision_type == CollisionType::kFull) {
    return PointInsideTile(point, tile_pos);
  }

  // TODO handle other collision types
  return false;
}

bool
PointColliding(const World& world, const Position point)
{
  WorldPos tile_pos = MeterPosToWorldPos(point);
  return PointCollideWithTile(world, point, tile_pos);
}

bool
OnGround(const World& world, const Position point)
{
  // check @offset pixel(s) below us
  constexpr f32 offset = kPixelInMeter * 1;
  const Position point_under(point.x, point.y - offset);
  const WorldPos tile_under = MeterPosToWorldPos(point_under);
  return PointCollideWithTile(world, point, tile_under);
}

/**
 * With a point coming from the point @from, have it be as close to the point
 * @to as possible. If tile at position @to has no collision, from = to. If
 * the tile at position @to has collision, place the point as close as
 * possible.
 *
 * @param from The position that we are currently at.
 * @param to Desired position. Tile at this position should have some collision
 */
static Position
MovePointNoCollide(Position from, Position to)
{
  const WorldPos collide_tile = MeterPosToWorldPos(to);

  // TODO
  constexpr float TMP_WIDTH = kTileInMeters;
  if (to.x < from.x) {
    // colliding tile to the left
    to.x = TileToMeter(collide_tile.X()) + kTileInMeters + TMP_WIDTH / 2.0f;
  } else if (to.x > from.x) {
    // colliding tile to the right
    to.x = TileToMeter(collide_tile.X()) - TMP_WIDTH / 2.0f;
  }

  if (to.y < from.y) {
    // colliding tile down
    to.y = TileToMeter(collide_tile.Y()) + kTileInMeters;
  } else if (to.y > from.y) {
    // colliding tile up
    // TODO
    constexpr float TMP_HEIGHT = kTileInMeters * 3;
    to.y = TileToMeter(collide_tile.Y()) - TMP_HEIGHT;
  }

  return to;
}

bool
MovePointCollideable(const World& world, Position& point, const Position target)
{
  // TODO handle more collision types than just tile?

  bool did_collide = false;

  // single-tile optimization
  constexpr f32 kCloseEnough = PixelToMeter(0.5f);
  constexpr f32 kJustOverOneTile = TileToMeter(1) + kCloseEnough;
  if (std::abs(point.x - target.x) < kJustOverOneTile &&
      std::abs(point.y - target.y) < kJustOverOneTile) {
    if (!PointColliding(world, target)) {
      point = target;
    } else {
      point = MovePointNoCollide(point, target);
      did_collide = true;
    }
  }
  // distance more than one tile
  else {
    const f32 diffx = target.x - point.x;
    const f32 diffy = target.y - point.y;
    const f32 signx = std::copysign(1.0f, diffx);
    const f32 signy = std::copysign(1.0f, diffy);
    const f32 dx = TileToMeter(1) * signx;
    const f32 dy = TileToMeter(1) * signy;
    for (;;) {

      bool any_changed = false;
      Position last_point = point;
      if (std::abs(target.x - point.x) > kCloseEnough) {
        any_changed = true;
        point.x += dx;
        if (PointColliding(world, point)) {
          did_collide = true;
        }
      }
      if ((dy < 0.0f && point.y + dy > target.y) /* down */ ||
          (dy > 0.0f && point.y + dy < target.y)) /* up */ {
        any_changed = true;
        point.y += dy;
        if (PointColliding(world, point)) {
          did_collide = true;
        }
      }

      if (!any_changed) {
        break;
      }

      if (did_collide) {
        point = MovePointNoCollide(last_point, point);
        break;
      }
    }
  }

  return did_collide;
}

void
MoveEntity(const World& world,
           const f64 delta,
           MoveableEntity* entity,
           const Acceleration h_acc,
           const Acceleration v_acc)
{
  const auto& terrain = world.GetTerrain();
  const Position old_position = entity->position;
  const bool on_ground = OnGround(world, entity->position);

  // TODO have player accelration be capped at some level, before extreme

  // calculate acceleration
  constexpr float kExtreme = kStandardGravity * kMaxAccelerationModifier;
  if (!alflib::FloatEqual(h_acc, 0.0f)) {
    const float max_acceleration =
      std::min(kExtreme, entity->horizontal_max_acceleration);
    entity->horizontal_acceleration =
      dutil::Clamp(entity->horizontal_acceleration + h_acc,
                   -max_acceleration,
                   max_acceleration);
  } else {
    // cancel out our acceleration
    constexpr f32 kAdjustmentFactor = 50.0f;
    if (entity->horizontal_acceleration < 0.0f) {
      entity->horizontal_acceleration += kAdjustmentFactor * delta;
      if (entity->horizontal_acceleration > 0.0f) {
        // cancel out over adjustments
        entity->horizontal_acceleration = 0.0f;
      }
    } else if (entity->horizontal_acceleration > 0.0f) {
      entity->horizontal_acceleration -= kAdjustmentFactor * delta;
      if (entity->horizontal_acceleration < 0.0f) {
        // cancel out over adjustments
        entity->horizontal_acceleration = 0.0f;
      }
    }
  }

  if (!alflib::FloatEqual(v_acc, 0.0f)) {
    entity->vertical_acceleration =
      dutil::Clamp(entity->vertical_acceleration + v_acc, -kExtreme, kExtreme);
  }

  Acceleration v_acc_gravity = entity->vertical_acceleration;
  if (!on_ground) {
    v_acc_gravity -= kStandardGravity;
  }

  // calculate velocity based on acceleration
  // TODO maby remove these checks? always add?
  bool has_h_acc = false;
  if (!alflib::FloatEqual(entity->horizontal_acceleration, 0.0f)) {
    has_h_acc = true;
    entity->horizontal_velocity += entity->horizontal_acceleration * delta;
  }
  if (!alflib::FloatEqual(v_acc_gravity, 0.0f)) {
    entity->vertical_velocity += v_acc_gravity * delta;
  }

  // calculate x position
  if (!alflib::FloatEqual(entity->horizontal_velocity, 0.0f)) {
    entity->position.x =
      dutil::Clamp(static_cast<f32>(entity->position.x +
                                    entity->horizontal_velocity * delta),
                   0.0f,
                   static_cast<f32>(terrain.GetWidth()));

    // check collision
    Position new_position = old_position;
    const bool did_collide =
      MovePointCollideable(world, new_position, entity->position);
    entity->position = new_position;

    if (did_collide) {
      entity->horizontal_acceleration = 0.0f;
      entity->horizontal_velocity = 0.0f;

      // move such that entity doesnt collide
      const WorldPos collide_tile = MeterPosToWorldPos(entity->position);
      if (entity->position.x < old_position.x) {
        // colliding tile to the left
        entity->position.x = dutil::Clamp(
          TileToMeter(collide_tile.X()) + kTileInMeters + entity->width / 2.0f,
          0.0f,
          static_cast<f32>(terrain.GetWidth()));
      } else {
        // colliding tile to the right
        entity->position.x =
          dutil::Clamp(TileToMeter(collide_tile.X()) - entity->width / 2.0f,
                       0.0f,
                       static_cast<f32>(terrain.GetWidth()));
      }

    } else if (on_ground) {
      // apply friction
      constexpr f32 kFrictionConstant = 5.0f;
      if (entity->horizontal_velocity < 0.0f) {
        entity->horizontal_velocity -=
          (entity->horizontal_velocity - kFrictionConstant) *
          entity->friction_modifier * delta;
        if (entity->horizontal_velocity > 0.0f) {
          // cancel out over adjustments
          entity->horizontal_velocity = 0.0f;
        }
      } else if (entity->horizontal_velocity > 0.0f) {
        entity->horizontal_velocity -=
          (entity->horizontal_velocity + kFrictionConstant) *
          entity->friction_modifier * delta;
        if (entity->horizontal_velocity < 0.0f) {
          // cancel out over adjustments
          entity->horizontal_velocity = 0.0f;
        }
      }
    }
  }

  // calculate y position
  if ((!on_ground || entity->vertical_velocity > 0.0f) &&
      !alflib::FloatEqual(entity->vertical_velocity, 0.0f)) {
    entity->position.y = dutil::Clamp(
      static_cast<f32>(entity->position.y + entity->vertical_velocity * delta),
      0.0f,
      static_cast<f32>(terrain.GetHeight()));

    // check collision
    Position new_position = old_position;
    const bool did_collide =
      MovePointCollideable(world, new_position, entity->position);
    entity->position = new_position;

    if (did_collide) {
      entity->vertical_acceleration = 0.0f;
      entity->vertical_velocity = 0.0f;

    } else {
      // apply friction / wind drag?
    }
  }
}

void
ForceOnEntity(MoveableEntity* entity,
              const f32 horizontal_force,
              const f32 vertical_force)
{
  entity->horizontal_velocity += horizontal_force;
  entity->vertical_velocity += vertical_force;
}
}
