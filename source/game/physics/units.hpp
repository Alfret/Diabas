#ifndef UNITS_HPP_
#define UNITS_HPP_

#include "core/types.hpp"
#include "game/world_pos.hpp"

namespace dib::game {

class World;

/**
 * Speed is the magnitude, direction is represented by the sign.
 */
using Velocity = f32;

/**
 * Position of something, specified in meters.
 */
using Position = Vector2F;

struct Force
{
  // horizontal
  Velocity h_vel;
  // vertical
  Velocity v_vel;
  // duration the force will be used, negative number means the duration
  // has passed.
  f32 duration_s;
};

constexpr f32 kStandardGravity = 9.80665f;

constexpr f32 kTileInPixels = 16.0f;
constexpr f32 kDoorHeightInTiles = 3.0f;
constexpr f32 kDoorHeightInMeters = 2.0f;
constexpr f32 kTileInMeters = kDoorHeightInMeters / kDoorHeightInTiles;
constexpr f32 kPixelInMeter = kTileInMeters / kTileInPixels;

constexpr f32
PixelToMeter(const f32 pixels)
{
  return pixels * kPixelInMeter;
}

/**
 * Note: lets say a tile is 1 meter, any input in the range [0, 1) will
 * give you 0.
 *
 * Example:
 *   MeterToTile(kTileInMeter * 0.5) -> 0
 *   MeterToTile(kTileInMeter * 1.5) -> 1
 *   MeterToTile(kTileInMeter * 1.99) -> 1
 *   MeterToTile(kTileInMeter * 2) -> 2
 */
inline u32
MeterToTile(const f32 meter)
{
  return static_cast<u32>(std::floor(meter / kTileInMeters));
}

constexpr f32
MeterToPixel(const f32 meters)
{
  return meters / kPixelInMeter;
}

constexpr f32
TileToPixel(const u32 tile)
{
  return tile * kTileInPixels;
}

constexpr f32
TileToMeter(const u32 tile)
{
  return tile * kTileInMeters;
}

/**
 * Take a position specified in meters, and translate to a WorldPos
 * specified in tile units.
 */
inline WorldPos
MeterPosToWorldPos(const Position meter_pos)
{
  WorldPos world_pos(MeterToTile(meter_pos.x), MeterToTile(meter_pos.y));
  return world_pos;
}

}

#endif // UNITS_HPP_
