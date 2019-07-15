#ifndef COLLIDEABLE_HPP_
#define COLLIDEABLE_HPP_

#include "core/types.hpp"
#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>

namespace dib::game {

/** Enumeration of collision types. This determines how entities collide with
 * the tile **/
enum class CollisionType : u8
{
  /** No collision **/
  kNone = 0,
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

#pragma pack(push, 1)
struct Collideable
{
  CollisionType type;
  static constexpr u32 kDataSize = 24;
  u8 reserved[kDataSize];

  bool ToBytes(alflib::RawMemoryWriter& mw) const
  {
    mw.Write(static_cast<u32>(type));
    return mw.WriteBytes(reserved, kDataSize);
  }

  static Collideable FromBytes(alflib::RawMemoryReader& mr)
  {
    Collideable data{};
    data.type = static_cast<CollisionType>(mr.Read<u32>());
    std::memcpy(data.reserved, mr.ReadBytes(kDataSize), kDataSize);
    return data;
  }
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CollideableRect
{
  CollisionType type;
  CollisionRectNoOffset rect;
  u8 padding[Collideable::kDataSize - sizeof(rect)];
};
#pragma pack(pop)
static_assert(sizeof(Collideable) == sizeof(CollideableRect));

#pragma pack(push, 1)
struct CollideableRect2
{
  CollisionType type;
  CollisionRectNoOffset rect1;
  /** x and y are offsets from the collideables origo **/
  CollisionRect rect2;
};
#pragma pack(pop)
static_assert(sizeof(Collideable) == sizeof(CollideableRect2));

}

#endif // COLLIDEABLE_HPP_
