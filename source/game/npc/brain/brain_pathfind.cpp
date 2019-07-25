#include "brain_pathfind.hpp"
#include "game/gameplay/moveable.hpp"
#include "game/world.hpp"
#include "game/physics/units.hpp"
#include "game/world_pos.hpp"
#include "game/physics/path_finding.hpp"
#include <dutil/stopwatch.hpp>

namespace dib::game {

bool
BrainPathfind::Think(World& world, f32 delta, Moveable& me, const Position target)
{
  auto& terrain = world.GetTerrain();

  const Position pa = me.position;
  const WorldPos start = MeterPosToWorldPos(pa);
  const WorldPos goal = MeterPosToWorldPos(target);

  // Generate a path to player
  astar_list_ = AStar(world, me, goal);

  if (world.GetDrawAStar()) {
    world.GetEntityManager().GetRegistry().assign_or_replace<AStarPathDebug>(
      entity_, AStarPathDebug{ astar_list_ });
  }

  //const bool can_follow = !astar_list_.empty();
  const bool can_follow = false;

  if constexpr (kSide == Side::kServer) {
    // Set the input
    me.input = GameInput{};
    if (can_follow) {
      if (astar_list_.front().X() < MeterToTile(me.position.x)) {
        me.input.ActionLeft();
      } else {
        me.input.ActionRight();
      }
      if (astar_list_.front().Y() > MeterToTile(me.position.y)) {
        me.input.ActionJump();
      }
    }
  }

  return can_follow;
}
}
