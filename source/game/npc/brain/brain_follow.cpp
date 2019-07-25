#include "brain_follow.hpp"
#include "game/gameplay/moveable.hpp"
#include "game/world.hpp"
#include "game/physics/units.hpp"
#include "game/world_pos.hpp"

namespace dib::game {

void
BrainFollow::Think(World& world, f32 delta, Moveable& me, const Position target)
{
  auto& terrain = world.GetTerrain();

  const Position pa = me.position;
  const WorldPos start = MeterPosToWorldPos(pa);
  const WorldPos goal = MeterPosToWorldPos(target);

  // Generate a path to player
  if constexpr (kSide == Side::kServer) {
    // Set the input
    me.input = GameInput{};
      if (target.x < me.position.x) {
        me.input.ActionLeft();
      } else {
        me.input.ActionRight();
      }

      if (target.y > me.position.y) {
        me.input.ActionJump();
      }
    }
}
}
