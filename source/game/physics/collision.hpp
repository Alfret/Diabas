#ifndef COLLISION_HPP_
#define COLLISION_HPP_

#include "core/types.hpp"
#include "game/physics/units.hpp"
#include "game/physics/collideable.hpp"
#include "game/gameplay/moveable.hpp"
#include <vector>
#include <optional>

namespace dib::game {

struct MoveableEntity;

struct CollisionInfo
{
  float x;
  float y;

  bool LeftCollision() const { return x < 0.0f; }
  bool RightCollision() const { return x > 0.0f; }
  bool UpCollision() const { return y > 0.0f; }
  bool DownCollision() const { return y < 0.0f; }
  bool HorizontalCollision() const
  {
    return LeftCollision() || RightCollision();
  }
  bool VerticalCollision() const { return UpCollision() || DownCollision(); }
  operator bool() const { return HorizontalCollision() or VerticalCollision(); }
};

/**
 * Is the @collidable colliding with any tiles when on @position?
 * @return The position of the first colliding tile, or nullopt.
 */
std::optional<WorldPos>
CollidesOnPosition(const World& world,
                   const Collideable& collideable,
                   Position position);

/**
 * Is the moveable touching the ground, aka standing, aka not falling.
 */
bool
OnGround(const World& world, const Moveable& moveable);

/**
 * Would the moveable stand on ground at position @pos?
 */
bool
OnGround(const World& world, const Moveable& moveable, const Position pos);

/**
 * Make a collection of positions on the range [@a, @b], in steps of
 * @d_stepsize. Useful for generating a set of possible collisions
 * when moving from @a to @b.
 */
void
GeneratePositions(const Position a,
                  const Position b,
                  std::vector<Position>& positions,
                  float d_stepsize = kTileInMeters);
}

#endif // COLLISION_HPP_
