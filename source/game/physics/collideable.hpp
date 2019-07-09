#ifndef COLLIDEABLE_HPP_
#define COLLIDEABLE_HPP_

#include "core/types.hpp"

namespace dib::game {

/** Enumeration of collision types. This determines how entities collide with
 * the tile **/
enum class CollisionType
{
  /** No collision **/
  kNone,
  /** Full tile collision **/
  kFullTile,
  /** Rectangular collision with one rectangle **/
  kRect,
  /** Rectangular collision with two rectangles **/
  kRect2,
  /** Stairs collision **/
  kStairs,
  /** Custom collision. 'Tile::GetAABB()' is called to determine collision **/
  kCustom
};

// ============================================================ //

// (performance) not use 4 bytes, instead 1 or 2 bytes and store the pixel
// distance. Use u8, u16 or f16?
struct CollisionRect
{
  f32 x;
  f32 y;
  f32 width;
  f32 height;
};

struct CollisionRectNoOffset
{
  f32 width;
  f32 height;
};

// ============================================================ //

/**
 * The collidable base class.
 *
 * Typical use case, you look at the type and cast it into what it should be.
 */
struct Collideable
{
  CollisionType type;
  u8 reserved[24];
};

struct CollideableRect
{
  CollisionType type;
  CollisionRectNoOffset rect;
  u8 padding[16];
};
static_assert(sizeof(Collideable) == sizeof(CollideableRect));

struct CollideableRect2
{
  CollisionType type;
  CollisionRectNoOffset rect1;
  /** x and y are offsets from the collideables origo **/
  CollisionRect rect2;
};
static_assert(sizeof(Collideable) == sizeof(CollideableRect2));

}

#endif // COLLIDEABLE_HPP_
