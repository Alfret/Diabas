#include "player.hpp"

#include "game/physics/units.hpp"
#include "game/client/game_client.hpp"
#include "game/gameplay/moveable.hpp"
#include <microprofile/microprofile.h>
#include <dutil/stopwatch.hpp>
#include "game/npc/npc_spawn.hpp"

namespace dib::game {

void
Player::Update(GameClient& game, [[maybe_unused]] const f64 delta)
{
  if constexpr (kSide == Side::kClient) {
    MICROPROFILE_SCOPEI("player", "update", MP_GREEN);

    auto& world = game.GetWorld();
    auto& network = world.GetNetwork();

    if (auto maybe_entity = network.GetOurPlayerEntity(); maybe_entity) {
      auto& registry = world.GetEntityManager().GetRegistry();
      Moveable& moveable = registry.get<Moveable>(*maybe_entity);
      moveable.input = GameInput{};

      if (game.IsKeyDown(Key::kKeyA)) {
        moveable.input.ActionLeft();
      }
      if (game.IsKeyDown(Key::kKeyD)) {
        moveable.input.ActionRight();
      }
      if (game.IsKeyDown(Key::kKeySpace)) {
        moveable.input.ActionJump();
      }
      if (game.IsKeyDown(Key::kKey1)) {
        static dutil::Stopwatch spawn_timer{};
        if (spawn_timer.now_ms() > 100) {
          spawn_timer.Start();
          NpcSpawn<kSide>(world, "slime", moveable.position);
        }
      }
      if (game.IsKeyDown(Key::kKey2)) {
        static dutil::Stopwatch spawn_timer{};
        if (spawn_timer.now_ms() > 100) {
          spawn_timer.Start();
          NpcSpawn<kSide>(world, "rabbit", moveable.position);
        }
      }
      if (game.IsKeyDown(Key::kKey3)) {
        static dutil::Stopwatch spawn_timer{};
        if (spawn_timer.now_ms() > 100) {
          spawn_timer.Start();
          NpcSpawn<kSide>(world, "blue blob", moveable.position);
        }
      }
    }
  }
}

}
