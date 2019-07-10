#include "player.hpp"

#include "game/physics/units.hpp"
#include "game/client/game_client.hpp"
#include "game/physics/moveable.hpp"
#include <microprofile/microprofile.h>

namespace dib::game {

void
Player::Update(GameClient& game, const f64 delta)
{
  MICROPROFILE_SCOPEI("player", "update moveable", MP_GREEN);

  auto& world = game.GetWorld();
  auto& network = world.GetNetwork();
  if (auto maybe_entity = network.GetOurPlayerEntity(); maybe_entity) {
    auto& registry = world.GetEntityManager().GetRegistry();
    Moveable& moveable = registry.get<Moveable>(*maybe_entity);

    f32 h_vel = 0.0f;
    f32 v_vel = 0.0f;

    bool action = false;
    if (game.IsKeyDown(Key::kKeyA)) {
      action = true;
      h_vel -= moveable.velocity_input;
    }
    if (game.IsKeyDown(Key::kKeyD)) {
      action = true;
      h_vel += moveable.velocity_input;
    }

    if (game.IsKeyDown(Key::kKeySpace)) {
      action = true;
      const bool on_ground = OnGround(world, moveable);
      if (on_ground && moveable.vertical_velocity == 0) {
        ForceOnMoveable(moveable, 0.0f, moveable.velocity_jump);
      }
      moveable.jumping = true;
    } else {
      moveable.jumping = false;
    }

    if (action) {
      UpdateMoveable(world, delta, moveable, h_vel, v_vel);
    }
  }

}

}
