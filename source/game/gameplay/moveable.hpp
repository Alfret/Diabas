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
struct MoveableIncrement
{
  f32 horizontal_velocity;
  f32 vertical_velocity;
  Force force;
  u8 jumping : 1;
  Position position;
  PlayerInput input;

  bool ToBytes(alflib::RawMemoryWriter& mw) const;

  static MoveableIncrement FromBytes(alflib::RawMemoryReader& mr);
};

// ============================================================ //

struct Moveable
{
  /**
   * negative -> left_
   * positive -> right
   */
  Velocity horizontal_velocity;

  /**
   * negative -> down
   * positive -> up
   */
  Velocity vertical_velocity;

  Force force;

  Velocity velocity_input;
  Velocity velocity_max;
  Velocity velocity_jump;
  u8 jumping : 1;

  /**
   * Unit: position is specified in meters
   * position origin is at middle x, bottom y, (right at the toes), of the
   * character
   */
  Position position;

  PlayerInput input;

  /**
   * Visual width and height.
   * Unit: pixels
   */
  s32 width;
  s32 height;

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

// ============================================================ //
// Functions
// ============================================================ //

/**
 * Simulate what all the moveables will do in the given time step.
 */
void
UpdateMoveables(World& world, f64 delta);

/**
 *
 */
void
ForceOnMoveable(Moveable& moveable, Force force);

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
  m.width = 24;
  m.height = 48;
  m.force.duration_s = -1.0f;
  m.velocity_input = 40.0f;
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
