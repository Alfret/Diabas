#include "player.hpp"

#include "game/physics/units.hpp"
#include "game/client/game_client.hpp"
#include <dutil/misc.hpp>
#include <alflib/math/math.hpp>
#include "core/fixed_time_update.hpp"
#include "game/physics/moveable.hpp"

#include <dutil/stopwatch.hpp>
#include "core/fixed_time_update.hpp"
#include <chrono>
#include <thread>

namespace dib::game {

void
Player::Update(GameClient& game, const f64 delta)
{
  auto& world = game.GetWorld();
  auto& network = world.GetNetwork();
  if (auto maybe_entity = network.GetOurPlayerEntity(); maybe_entity) {
    auto& registry = world.GetEntityManager().GetRegistry();
    Moveable& moveable = registry.get<Moveable>(*maybe_entity);

    Acceleration current_h_acc = 0.0f;
    Acceleration current_v_acc = 0.0f;

    /// PLAYGROUND

    /// ---------------------

    f32 h_force = 0.0f;
    if (game.IsKeyDown(Key::kKeyA)) {
      h_force = -5.0f;
      current_h_acc -= moveable.horizontal_acceleration_modifier * delta;
    }
    if (game.IsKeyDown(Key::kKeyD)) {
      h_force = 5.0f;
      current_h_acc += moveable.horizontal_acceleration_modifier * delta;
    }

    if (game.IsKeyDown(Key::kKeySpace)) {
      f32 v_force = 15.0f;
      const bool on_ground = OnGround(world, moveable);
      if (on_ground) {
        ForceOnMoveable(moveable, h_force, v_force);
      }
    }

    MoveMoveable(world, delta, moveable, current_h_acc, current_v_acc);
  }
}

}
