#ifndef PATH_FINDING_HPP_
#define PATH_FINDING_HPP_

#include "game/world_pos.hpp"
#include <vector>

namespace dib::game {

class World;
class Moveable;

struct AStarPathDebug
{
  std::vector<WorldPos> path;
};

struct Node
{
  WorldPos pos;
  WorldPos origin;
  s32 g;
  s32 h;
  s8 jump_x;
  s8 jump_y;
  s8 did_jump_x;
  constexpr s32 F() const { return g + h; }
};

std::vector<WorldPos>
AStar(const World& world,
      const Moveable& moveable,
      WorldPos goal);
}

#endif // PATH_FINDING_HPP_
