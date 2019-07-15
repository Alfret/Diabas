#ifndef MOVEABLE_HPP_
#define MOVEABLE_HPP_

#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>
#include "game/physics/units.hpp"
#include "core/types.hpp"
#include "game/physics/collideable.hpp"
#include "game/gameplay/player.hpp"

namespace dib::game {

// ============================================================ //
// Structs
// ============================================================ //

/**
 * This structure will be sent often over network. Contain the fast changing
 * variables of the moveable.
 */
#pragma pack(push, 1)
struct MoveableIncrement
{
  f32 horizontal_velocity;
  f32 vertical_velocity;
  u8 jumping : 1;
  Position position;
  PlayerInput input;

  bool ToBytes(alflib::RawMemoryWriter& mw) const;

  static MoveableIncrement FromBytes(alflib::RawMemoryReader& mr);
};
#pragma pack(pop)

// ============================================================ //

#pragma pack(push, 1)
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
  u8 jumping : 1;

  /**
   * position is specified in meters
   * position origin is at middle x, bottom y, (right at the toes), of the
   * character
   */
  Position position;

  PlayerInput input;

  /**
   * Visual width and height
   */
  f32 width;
  f32 height;

  /**
   * Physical footprint of the moveable.
   */
  Collideable collideable;

  // ============================================================ //

  MoveableIncrement ToIncrement() const;

  void FromIncrement(const MoveableIncrement& m);

  bool ToBytes(alflib::RawMemoryWriter& mw) const;

  static Moveable FromBytes(alflib::RawMemoryReader& mr);
};
#pragma pack(pop)

// ============================================================ //
// Functions
// ============================================================ //

/**
 * Simulate what all the moveables will do in the given time step.
 */
void
UpdateMoveables(World& world, f64 delta);

/**
 * Apply a force to a moveable. This means, instantly, modifying the entities
 * velocity.
 */
void
ForceOnMoveable(Moveable& moveable, f32 horizontal_force, f32 vertical_force);

/**
 * Apply the incremental update to the given moveable.
 */
void
ApplyMoveableIncrement(Moveable& moveable,
                       MoveableIncrement increment,
                       bool is_our_moveable);

/**
 * To be removed when players can be created and stored in a better way.
 */
inline Moveable
MoveableMakeDefault()
{
  Moveable m{};
  m.width = game::kTileInMeters * 1.5f;
  m.height = game::kTileInMeters * 2.9f;
  m.velocity_input = 20.0f;
  m.velocity_max = 10.0f;
  m.velocity_jump = 14.0f;
  m.jumping = false;
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
