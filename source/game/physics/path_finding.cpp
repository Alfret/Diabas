#include "path_finding.hpp"
#include "game/world.hpp"
#include "game/gameplay/moveable.hpp"
#include <optional>

namespace dib::game {

// ============================================================ //
// Path finding with modified A*
// ============================================================ //
/// we pick next node depending on f value, the sum g and h.
/// g = current total cost to get to this node
/// h = estimated cost to get from this node, to final node. (Manhattan).
std::vector<WorldPos>
AStar(const World& world, const Moveable& moveable, const WorldPos goal)
{
  const WorldPos start = MeterPosToWorldPos(moveable.position);
  const auto& terrain = world.GetTerrain();

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

  const s32 max_jump_y = JumpVelocityToTiles(moveable.velocity_jump);
  const s32 max_jump_x = 3; // TMP value
  constexpr s32 kMaxLoopCount = 500;
  s32 loop_count = 0;

  // 1. init lists
  std::vector<Node> open_list{};
  std::vector<Node> closed_list{};
  std::vector<Node> successors{};

  // 2. put starting node on open list (/w f=0)
  open_list.push_back({ start, start, 0, 0, 0, 0 });

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
      // left
      if (q.pos.X() > 0) {
        Node left = Node{ q.pos.Left(), q.pos, q.g + 1, 0, q.jump_x, q.jump_y};
        if (!CollidesOnPosition(
               world, moveable.collideable, WorldPosToMeterPos(left.pos))
               .has_value()) {
          // left with ground under
          if (OnGround(world, moveable, WorldPosToMeterPos(left.pos))) {
            left.h = ManhattanDist(left.pos, goal);
            left.jump_x = 0;
            left.jump_y = 0;
            successors.push_back(left);
          }
          // left with jump
          if (q.jump_x < max_jump_x) {
            left.h = ManhattanDist(left.pos, goal);
            left.jump_x = q.jump_x + 1;
            // left up
            if (q.jump_y < max_jump_y) {
              left.jump_y = q.jump_y + 1;
              if (left.pos.Y() < terrain.GetHeight()) {
                Node left_jump = left;
                left_jump.pos = left.pos.Top();
                if (!CollidesOnPosition(
                        world, moveable.collideable, WorldPosToMeterPos(left_jump.pos))
                    .has_value()) {
                  successors.push_back(left_jump);
                }
              }
            }
            // left down
            else {
              left.jump_y = q.jump_y;
              if (left.pos.Y() > 0) {
                Node left_fall = left;
                left_fall.pos = left.pos.Bottom();
                if (!CollidesOnPosition(world,
                                        moveable.collideable,
                                        WorldPosToMeterPos(left_fall.pos))
                       .has_value()) {
                  successors.push_back(left_fall);
                }
              }
            }
          }
        }
      }

      // right
      if (q.pos.X() < terrain.GetWidth()) {
        Node right = Node{ q.pos.Right(), q.pos, q.g + 1, 0, q.jump_x, q.jump_y};
        if (!CollidesOnPosition(
               world, moveable.collideable, WorldPosToMeterPos(right.pos))
               .has_value()) {
          // right with ground under
          if (OnGround(world, moveable, WorldPosToMeterPos(right.pos))) {
            right.h = ManhattanDist(right.pos, goal);
            right.jump_x = 0;
            right.jump_y = 0;
            successors.push_back(right);
          }
          // right with jump
          if (q.jump_x < max_jump_x) {
            right.h = ManhattanDist(right.pos, goal);
            right.jump_x = q.jump_x + 1;
            // right up
            if (q.jump_y < max_jump_y) {
              right.jump_y = q.jump_y + 1;
              if (right.pos.Y() < terrain.GetHeight()) {
                Node right_jump = right;
                right_jump.pos = right.pos.Top();
                if (!CollidesOnPosition(
                        world, moveable.collideable, WorldPosToMeterPos(right_jump.pos))
                    .has_value()) {
                  successors.push_back(right_jump);
                }
              }
            }
            // right down
            else {
              right.jump_y = q.jump_y;
              if (right.pos.Y() > 0) {
                Node right_fall = right;
                right_fall.pos = right.pos.Bottom();
                if (!CollidesOnPosition(world,
                                        moveable.collideable,
                                        WorldPosToMeterPos(right_fall.pos))
                       .has_value()) {
                  successors.push_back(right_fall);
                }
              }
            }
          }
        }
      }

      // down
      if (q.pos.Y() > 0) {
        Node down = Node { q.pos.Bottom(), q.pos, q.g + 1, 0, 0, 0};
        if (!CollidesOnPosition(
               world, moveable.collideable, WorldPosToMeterPos(down.pos))
               .has_value()) {
          down.h = ManhattanDist(down.pos, goal);
          down.jump_y = max_jump_y;
          down.jump_x = q.jump_x;
          successors.push_back(down);
        }
      }

      // up
      if (q.pos.Y() < terrain.GetHeight()) {
        Node up = Node{ q.pos.Top(), q.pos, q.g + 1, 0, 0, 0 };
        if (!CollidesOnPosition(
               world, moveable.collideable, WorldPosToMeterPos(up.pos))
               .has_value()) {
          if ((q.jump_y == 0 &&
              OnGround(world, moveable, WorldPosToMeterPos(q.pos))) or
              (q.jump_y > 0 && q.jump_y < max_jump_y)) {
            up.h = ManhattanDist(up.pos, goal);
            up.jump_y = q.jump_y + 1;
            up.jump_x = q.jump_x;
            successors.push_back(up);
          }
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
          maybe_node && maybe_node->F() <= successor.F()) {
        continue;
      }

      // iii if already on closed list /w lower f, skip
      if (auto maybe_node = FindNode(closed_list, successor);
          maybe_node && maybe_node->F() <= successor.F()) {
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
