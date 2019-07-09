#include "collision.hpp"
#include "game/world.hpp"
#include <dutil/misc.hpp>
#include "game/physics/moveable.hpp"
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
  const u32 row = rows-1;
  for (u32 col = 0; col < cols; col++) {
    tiles.push_back(MeterPosToWorldPos(
        { dutil::Clamp(origo.x + d * col, 0.0f, maxx),
          dutil::Clamp(origo.y + d * row + remainy, 0.0f, maxy) }));
  }

  // fill out the right most column
  const u32 col = cols-1;
  for (u32 row = 0; row < rows; row++) {
    tiles.push_back(MeterPosToWorldPos(
        { dutil::Clamp(origo.x + d * col + remainx, 0.0f, maxx),
          dutil::Clamp(origo.y + d * row, 0.0f, maxy) }));
  }

  // top right one
  tiles.push_back(MeterPosToWorldPos(
    { dutil::Clamp(origo.x + d * (cols-1) + remainx, 0.0f, maxx),
      dutil::Clamp(origo.y + d * (rows-1) + remainy, 0.0f, maxy) }));
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
  bool collides = CollidesOnTiles(world, rect, tiles);
  if (collides) {
    Position col_dir(0.0f, 0.0f);

  }

  return collides;
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
                  std::vector<Position>& positions)
{
  const f32 width = b.x - a.x;
  const f32 height = b.y - a.y;
  const f32 remainx = std::fmod(width, kTileInMeters);
  const f32 remainy = std::fmod(height, kTileInMeters);
  f32 dx = (width - remainx)  / kTileInMeters;
  f32 dy = (height - remainy) / kTileInMeters;
  u32 steps;

  // select a delta such that the largest of dx and dy is kTileInMeters.
  if (std::abs(width) > std::abs(height) && std::abs(width) > kTileInMeters) {
    steps = static_cast<u32>(std::ceil(std::abs(width / kTileInMeters)));
    dy = kTileInMeters * (dy / std::abs(dx));
    dx = kTileInMeters * (dx < 0.0f ? -1.0f : 1.0f);
  } else if (std::abs(height) > kTileInMeters){
    steps = static_cast<u32>(std::ceil(std::abs(height / kTileInMeters)));
    dx = kTileInMeters * (dx / std::abs(dy));
    dy = kTileInMeters * (dy < 0.0f ? -1.0f : 1.0f);
  } else {
    constexpr f32 almost_pixel = kPixelInMeter * 0.99;
    steps = std::abs(width) > almost_pixel || std::abs(height) > almost_pixel
      ? 1 : 0;
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
    { a.x + dx * (steps-1) + remainx, a.y + dy * (steps-1) + remainy });
}

// ============================================================ //

// CollisionResult
// TryMoveEntity(const World& world, MoveableEntity& entity, const Position
// target)
// {
//   // TODO handle more collision types than just tile?

//   CollisionResult result{ false, false };
//   bool changed_position;
//   Position last_position;
//   const f32 diffx = target.x - entity.position.x;
//   const f32 diffy = target.y - entity.position.y;
//   const f32 signx = std::copysign(1.0f, diffx);
//   const f32 signy = std::copysign(1.0f, diffy);
//   f32 dx = diffx / kTileInMeters;
//   f32 dy = diffy / kTileInMeters;
//   f32 remainx = std::remainderf(dx, kTileInMeters);
//   f32 remainy = std::remainderf(dy, kTileInMeters);
//   dx = std::trunc(dx);
//   dy = std::trunc(dy);

//   // figure out the step size, such that the largest of dx and dy
//   // will be kTileInMeters.
//   if (std::abs(dx) > std::abs(dy) ) {
//     dy = kTileInMeters * (dy / std::abs(dx));
//     dx = kTileInMeters * signx;
//   } else if (std::abs(dx) > kPixelInMeter - kPixelInMeter / 100.0f){
//     dx = kTileInMeters * (dx / std::abs(dy));
//     dy = kTileInMeters * signy;
//   } else {
//     // Both are very small, make them big such that the algorithm below will
//     // ignore them.
//     dx = kTileInMeters;
//     dy = kTileInMeters;
//   }

//   // Move with tile precision
//   for (;;) {
//     changed_position = false;
//     last_position = entity.position;

//     // if ((dx < 0.0f && entity.position.x + dx -
//     //      entity.collision.rect.width / 2.0f > target.x)  /* left */ ||
//     //     (dx > 0.0f && entity.position.x + dx +
//     //      entity.collision.rect.width / 2.0f < target.x)) /* right */  {
//     //   changed_position = true;
//     //   entity.position.x += dx;
//     //   if (CheckCollision(world, entity.position, entity.collision)) {
//     //     result.x_collision = true;
//     //   }
//     // }
//     // if ((dy < 0.0f && entity.position.y + dy > target.y)  /* down */ ||
//     //     (dy > 0.0f && entity.position.y + dy +
//     //      entity.collision.rect.height < target.y)) /* up */ {
//     //   changed_position = true;
//     //   entity.position.y += dy;
//     //   if (CheckCollision(world, entity.position, entity.collision)) {
//     //     result.y_collision = true;
//     //   }
//     // }

//     if ((dx < 0.0f && entity.position.x + dx > target.x)  /* left */ ||
//         (dx > 0.0f && entity.position.x + dx < target.x)) /* right */  {
//       changed_position = true;
//       entity.position.x += dx;
//       if (CheckCollision(world, entity.position, entity.collision)) {
//         result.x_collision = true;
//       }
//     }
//     if ((dy < 0.0f && entity.position.y + dy > target.y)  /* down */ ||
//         (dy > 0.0f && entity.position.y + dy < target.y)) /* up */ {
//       changed_position = true;
//       entity.position.y += dy;
//       if (CheckCollision(world, entity.position, entity.collision)) {
//         result.y_collision = true;
//       }
//     }

//     if (!changed_position) {
//       // move with sub tile precision
//       entity.position.x += remainx;
//       entity.position.y += remainy;
//       if (CheckCollision(world, entity.position, entity.collision)) {
//         if (remainx > kPixelInMeter - kPixelInMeter / 100.0) {
//           result.x_collision = true;
//         }
//         if (remainy > kPixelInMeter - kPixelInMeter / 100.0) {
//           result.y_collision = true;
//         }
//       } else {
//         break;
//       }
//     }

//     if (result.y_collision || result.x_collision) {
//       // reposition entity beside the collided tile

//       const Position collided_position = entity.position;
//       const WorldPos collided_tile = MeterPosToWorldPos(collided_position);
//       const f32 half_width = entity.collision.rect.width / 2.0f;

//       if (result.x_collision) {
//         if (last_position.x + half_width > collided_position.x) {
//           // colliding tile to the left
//           entity.position.x =
//             TileToMeter(collided_tile.X()) - entity.collision.rect.width
//             / 2.0f;
//         } else if (last_position.x - half_width < collided_position.x) {
//           // colliding tile to the right
//           entity.position.x = TileToMeter(collided_tile.X()) + kTileInMeters
//           +
//                               entity.collision.rect.width / 2.0f;
//         }
//       }
//       else
//       {
//         // did not collide on this axis
//         entity.position.x = last_position.x;
//       }

//       if (result.y_collision) {
//         if (last_position.y + entity.collision.rect.height >
//             collided_position.y) {
//           // colliding tile up
//           entity.position.y =
//             TileToMeter(collided_tile.Y()) - entity.collision.rect.height;
//         } else if (last_position.y < collided_position.y) {
//           // collding tile down
//           entity.position.y = TileToMeter(collided_tile.Y()) + kTileInMeters;
//         }
//       } else {
//         // did not collide on this axis
//         entity.position.y = last_position.y;
//       }

//       break;
//     }
//   }

//   return result;
// }

}
