#include "path_finding.hpp"
#include "game/terrain.hpp"
#include <optional>

namespace dib::game {

// ============================================================ //
// Path finding with modified A*
// ============================================================ //
/// we pick next node depending on f value, the sum g and h.
/// g = current total cost to get to this node
/// h = estimated cost to get from this node, to final node. (Manhattan).
std::vector<WorldPos>
AStar(const WorldPos start, const WorldPos goal, const Terrain& terrain)
{
  const auto ManhattanDist = [](WorldPos node, WorldPos goal) {
    const s32 nx = static_cast<s32>(node.X());
    const s32 ny = static_cast<s32>(node.Y());
    const s32 gx = static_cast<s32>(goal.X());
    const s32 gy = static_cast<s32>(goal.Y());
    return static_cast<u32>(std::abs(nx - gx) + std::abs(ny - gy));
  };

  const auto FindNode = [](const std::vector<Node>& list,
                           Node node) -> std::optional<Node> {
    for (const auto n : list) {
      if (n.pos == node.pos) {
        return n;
      }
    }
    return std::nullopt;
  };

  constexpr s32 kMaxLoopCount = 100;
  s32 loop_count = 0;

  // 1. init lists
  std::vector<Node> open_list{};
  std::vector<Node> closed_list{};
  std::vector<Node> successors{};

  // 2. put starting node on open list (/w f=0)
  open_list.push_back({ start, start, 0, 0 });

  // 3. while open list not empty
  while (open_list.size() > 0) {
    if (++loop_count > kMaxLoopCount) {
      break;
    }

    // a) find node with least f on open list - call it q
    auto qit = open_list.begin();
    for (auto it = open_list.begin(); it < open_list.end(); it++) {
      if (it->g + it->h < qit->g + qit->h) {
        qit = it;
      }
    }

    // b) pop q off the open list and put on closed list
    const auto q = *qit;
    open_list.erase(qit);
    closed_list.push_back(q);

    // c) generate q's successors and set their parents to q
    successors.clear();
    {
      if (q.pos.X() > 0) {
        Node left = Node{ q.pos.Left(), q.pos, q.g + 1, 0 };
        if (!terrain.GetTile(left.pos)->GetCollisionIsSolid()) {
          left.h = ManhattanDist(left.pos, goal);
          successors.push_back(left);
        }
      }

      if (q.pos.X() < terrain.GetWidth()) {
        Node right = Node{ q.pos.Right(), q.pos, q.g + 1, 0 };
        if (!terrain.GetTile(right.pos)->GetCollisionIsSolid()) {
          right.h = ManhattanDist(right.pos, goal);
          successors.push_back(right);
        }
      }
    }

    // d) for each successor
    for (auto successor : successors) {

      // i   if successor is goal, stop
      if (successor.pos == goal) {
        closed_list.push_back(successor);
        open_list.clear();
        break;
      }

      // ii  if already on open list /w lower f, skip
      if (auto maybe_node = FindNode(open_list, successor);
          maybe_node && maybe_node->F() < successor.F()) {
        continue;
      }

      // iii if already on closed list /w lower f, skip
      if (auto maybe_node = FindNode(closed_list, successor);
          maybe_node && maybe_node->F() < successor.F()) {
        continue;
      } else {
        open_list.push_back(successor);
      }
    }
  }

  // build correct path
  std::vector<WorldPos> astar_list{};

  if (!closed_list.empty() and closed_list.back().pos == goal) {
    auto& node = closed_list.back();
    WorldPos next = node.pos;
    for (u32 i = closed_list.size(); i > 0; i--) {
      if (next == closed_list[i].pos) {
        astar_list.push_back(closed_list[i].pos);
        next = closed_list[i].origin;
      }
    }
  }

  return astar_list;
}
}
