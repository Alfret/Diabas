#include "brain_follow.hpp"
#include "game/gameplay/moveable.hpp"
#include "game/world.hpp"
#include "game/physics/units.hpp"
#include "game/world_pos.hpp"
#include "game/physics/path_finding.hpp"

namespace dib::game {

void
BrainFollow::Think(World& world, f32 delta, Moveable& me, const Position target)
{
  auto& terrain = world.GetTerrain();

  const Position pa = me.position;
  const s32 max_jump_height = JumpVelocityToTiles(me.velocity_jump);
  const s32 max_jump_width = 1; // TMP value
  const s32 tile_dist_x = MeterToTile(target.x) - MeterToTile(me.position.x);
  const s32 tile_dist_y = MeterToTile(target.y) - MeterToTile(me.position.y);

  const WorldPos start = MeterPosToWorldPos(pa);
  const WorldPos goal = MeterPosToWorldPos(target);

  // Generate a path to player
  std::vector<WorldPos> astar_list = AStar(start, goal, terrain);
  if (world.GetDrawAStar()) {
    world.GetEntityManager().GetRegistry().assign_or_replace<AStarPathDebug>(
      entity_, AStarPathDebug{ astar_list });
  }
  if constexpr (kSide == Side::kServer) {
    // Set the input
    me.input = GameInput{};
    if (astar_list.size() > 0) {
      if (astar_list.front().X() < MeterToTile(me.position.x)) {
        me.input.ActionLeft();
      } else {
        me.input.ActionRight();
      }
    }
  }
}
}
