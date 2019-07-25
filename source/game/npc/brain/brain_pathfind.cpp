#include "brain_pathfind.hpp"
#include "game/gameplay/moveable.hpp"
#include "game/world.hpp"
#include "game/physics/units.hpp"
#include "game/world_pos.hpp"
#include "game/physics/path_finding.hpp"
#include <dutil/stopwatch.hpp>

namespace dib::game {

static void
GameInputFromPath(const std::vector<WorldPos>& path, Moveable& m)
{
  GameInput input{};

  if (!path.empty()) {
    Position tpos{ TileToMeter(path.back().X()), TileToMeter(path.back().Y()) };
    if (path.size() > 1) {
      tpos =
        Position{ (tpos.x + TileToMeter(path[path.size() - 2].X())) / 2.0f,
                  (tpos.y + TileToMeter(path[path.size() - 2].Y())) / 2.0f };
    }

    if (tpos.x <m.position.x) {
      input.ActionLeft();
    } else {
      input.ActionRight();
    }
    if (tpos.y > m.position.y or (tpos.y +kTileInMeters - kPixelInMeter > m.position.y and m.input.Jump()) or
        (path.size() > 1 and
         path[path.size() - 2].Y() > MeterToTile(m.position.y))) {
      input.ActionJump();
      // cancel right movement
      if (path.size() > 1 and m.horizontal_velocity > 0.0f and
          path[path.size()-2].Y() <= path[path.size()-1].Y()) {
        input.ActionLeft();
      }
      // cancel left movement
      else if (path.size() > 1 and m.horizontal_velocity < 0.0f and
          path[path.size() - 2].Y() >= path[path.size() - 1].Y()) {
        input.ActionRight();
      }
    }
  }

  m.input = input;
}

bool
BrainPathfind::Think(World& world, f32, Moveable& me, const Position target)
{
  const WorldPos goal = MeterPosToWorldPos(target);

  // gen new path if not moving, or failing to get to next path node
  if ((alflib::FloatEqual(me.horizontal_velocity, 0.0f) &&
       alflib::FloatEqual(me.vertical_velocity, 0.0f)) or
      world.GetTime() - path_timer_ > 0.5) {
    path_timer_ = world.GetTime();
      path_ = AStar(world, me, goal);
  }
  // remove node in path if we are on it
  if (!path_.empty()) {
    if (path_.back() == MeterPosToWorldPos(me.position)) {
      path_timer_ = world.GetTime();
      path_.pop_back();
    }
  }

  if (world.GetDrawAStar()) {
    //TODO store path in moveable
    world.GetEntityManager().GetRegistry().assign_or_replace<AStarPathDebug>(
      entity_, AStarPathDebug{ path_ });
  }

  const bool can_follow = !path_.empty();
  //const bool can_follow = false;

  if constexpr (kSide == Side::kServer) {
    // Set the input
      GameInputFromPath(path_, me);
  }

  return can_follow;
}
}
