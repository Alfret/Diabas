#include "collision.hpp"
#include "game/world.hpp"
#include <dutil/misc.hpp>
#include <cmath>

namespace dib::game {

/**
 * Axis-Aligned Bounding Box Collision Detection - Are two boxes colliding?
 */
static bool
AABBCollisionDetection(const CollisionRect& a, const CollisionRect& b)
{
  return (a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height &&
          a.y + a.height > b.y);
}

/**
 * Does the @rect collide with any tile in @tiles?
 */
static bool
CollidesOnTiles(const World& world,
                const CollisionRect& rect,
                const std::vector<WorldPos>& tiles)
{
  bool colliding = false;

  for (const auto tile : tiles) {
    const auto tileptr = world.GetTerrain().GetTile(tile);
    const auto collision = tileptr->GetCollision(world, tile);

    if (collision == CollisionType::kNone) {
      continue;
    } else if (collision == CollisionType::kFullTile) {
      const CollisionRect tile_rect{ TileToMeter(tile.X()),
                                     TileToMeter(tile.Y()),
                                     kTileInMeters,
                                     kTileInMeters };
      if (AABBCollisionDetection(rect, tile_rect)) {
        colliding = true;
        break;
      }
    } else if (collision == CollisionType::kStairs) {
      AlfAssert(false, "cannot handle collision type Stairs for tile");
    } else {
      AlfAssert(false, "cannot handle collision type for tile");
    }
  }

  return colliding;
}

/**
 * From a @rect, with a given @origo, put the tiles that it is overlapping
 * with in @tiles.
 */
static void
GenerateTiles(const World& world,
              const CollisionRect& rect,
              const Position origo,
              std::vector<WorldPos>& tiles)
{
  constexpr f32 d = kTileInMeters;
  const f32 maxx = TileToMeter(world.GetTerrain().GetWidth() - 1);
  const f32 maxy = TileToMeter(world.GetTerrain().GetHeight() - 1);
  const u32 rows = static_cast<u32>(std::ceil(rect.height / kTileInMeters));
  const u32 cols = static_cast<u32>(std::ceil(rect.width / kTileInMeters));
  const f32 remainx = std::fmod(rect.height, kTileInMeters);
  const f32 remainy = std::fmod(rect.width, kTileInMeters);

  // @PERF we almost always add duplicate tiles, don't do that!

  // Add up tiles in increments of @d
  for (u32 row = 0; row < rows; row++) {
    for (u32 col = 0; col < cols; col++) {
      tiles.push_back(
        MeterPosToWorldPos({ dutil::Clamp(origo.x + d * col, 0.0f, maxx),
                             dutil::Clamp(origo.y + d * row, 0.0f, maxy) }));
    }
  }

  // fill out the top row
  const u32 row = rows - 1;
  for (u32 col = 0; col < cols; col++) {
    tiles.push_back(MeterPosToWorldPos(
      { dutil::Clamp(origo.x + d * col, 0.0f, maxx),
        dutil::Clamp(origo.y + d * row + remainy, 0.0f, maxy) }));
  }

  // fill out the right most column
  const u32 col = cols - 1;
  for (u32 row = 0; row < rows; row++) {
    tiles.push_back(MeterPosToWorldPos(
      { dutil::Clamp(origo.x + d * col + remainx, 0.0f, maxx),
        dutil::Clamp(origo.y + d * row, 0.0f, maxy) }));
  }

  // top right one
  tiles.push_back(MeterPosToWorldPos(
    { dutil::Clamp(origo.x + d * (cols - 1) + remainx, 0.0f, maxx),
      dutil::Clamp(origo.y + d * (rows - 1) + remainy, 0.0f, maxy) }));
}

static bool
CollidesOnPosition(const World& world,
                   const CollideableRect* collideable,
                   const Position position)
{
  std::vector<WorldPos> tiles;
  const CollisionRect rect{
    position.x, position.y, collideable->rect.width, collideable->rect.height
  };
  GenerateTiles(world, rect, position, tiles);
  return CollidesOnTiles(world, rect, tiles);
}

static bool
CollidesOnPosition(const World& world,
                   const CollideableRect2* collideable,
                   const Position position)
{
  std::vector<WorldPos> tiles;
  const CollisionRect rect1{
    position.x, position.y, collideable->rect1.width, collideable->rect1.height
  };
  GenerateTiles(world, rect1, position, tiles);
  if (!CollidesOnTiles(world, rect1, tiles)) {

    tiles.clear();
    const CollisionRect rect2{ position.x + collideable->rect2.x,
                               position.y + collideable->rect2.y,
                               collideable->rect2.width,
                               collideable->rect2.height };
    GenerateTiles(world, rect2, position, tiles);
    return CollidesOnTiles(world, rect2, tiles);
  }
  return true;
}

bool
CollidesOnPosition(const World& world,
                   const Collideable& collideable,
                   const Position position)
{
  if (collideable.type == CollisionType::kRect) {
    const auto c = reinterpret_cast<const CollideableRect*>(&collideable);
    return CollidesOnPosition(world, c, position);
  } else if (collideable.type == CollisionType::kRect2) {
    const auto c = reinterpret_cast<const CollideableRect2*>(&collideable);
    return CollidesOnPosition(world, c, position);
  } else {
    AlfAssert(false, "cannot check CollidesOnPosition for given CollisionType");
    return false;
  }
}

bool
OnGround(const World& world, const Moveable& moveable)
{
  // check @offset pixel(s) below us
  constexpr f32 offset = kPixelInMeter * 1;
  const Position pos_under(moveable.position.x, moveable.position.y - offset);
  return CollidesOnPosition(world, moveable.collideable, pos_under);
}

/**
 *
 * @pre a and b must correspond to valid tiles.
 */
void
GeneratePositions(const Position a,
                  const Position b,
                  std::vector<Position>& positions,
                  const float d_stepsize)
{
  const f32 width = b.x - a.x;
  const f32 height = b.y - a.y;
  const f32 remainx = std::fmod(width, d_stepsize);
  const f32 remainy = std::fmod(height, d_stepsize);
  f32 dx = (width - remainx) / d_stepsize;
  f32 dy = (height - remainy) / d_stepsize;
  u32 steps;

  // select a delta such that the largest of dx and dy is d_stepsize.
  if (std::abs(width) > std::abs(height) && std::abs(width) > d_stepsize) {
    steps = static_cast<u32>(std::ceil(std::abs(width / d_stepsize)));
    dy = d_stepsize * (dy / std::abs(dx));
    dx = d_stepsize * (dx < 0.0f ? -1.0f : 1.0f);
  } else if (std::abs(height) > d_stepsize) {
    steps = static_cast<u32>(std::ceil(std::abs(height / d_stepsize)));
    dx = d_stepsize * (dx / std::abs(dy));
    dy = d_stepsize * (dy < 0.0f ? -1.0f : 1.0f);
  } else {
    constexpr f32 almost_pixel = kPixelInMeter * 0.99;
    steps =
      std::abs(width) > almost_pixel || std::abs(height) > almost_pixel ? 1 : 0;
    dx = 0;
    dy = 0;
  }

  // Add up tiles in increments of @dx, @dy
  u32 step = 0;
  for (; step < steps; step++) {
    // @PERF only push back if it doesnt already exist?
    positions.push_back({ a.x + dx * step, a.y + dy * step });
  }
  // Add the remaining point
  positions.push_back(
    { a.x + dx * (steps - 1) + remainx, a.y + dy * (steps - 1) + remainy });
}

}
