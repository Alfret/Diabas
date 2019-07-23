#ifndef PATH_FINDING_HPP_
#define PATH_FINDING_HPP_

#include "game/world_pos.hpp"
#include <vector>

namespace dib::game {

class Terrain;

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
  constexpr s32 F() const { return g + h; }
};

std::vector<WorldPos>
AStar(const WorldPos start, const WorldPos goal, const Terrain& terrain);

}

#endif // PATH_FINDING_HPP_
