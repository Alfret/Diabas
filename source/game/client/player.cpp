#include "player.hpp"

#include "game/physics/units.hpp"
#include "game/client/game_client.hpp"
#include <dutil/misc.hpp>
#include <alflib/math/math.hpp>
#include "core/fixed_time_update.hpp"

namespace dib::game {

void
Player::Update(GameClient& game, const f64 delta)
{
  auto& world = game.GetWorld();
  auto& network = world.GetNetwork();
  if (auto maybe_player_data = network.GetOurPlayerData(); maybe_player_data) {
    PlayerData* player_data = *maybe_player_data;

    Acceleration current_h_acc = 0.0f;
    Acceleration current_v_acc = 0.0f;

    f32 h_force = 0.0f;
    if (game.IsKeyDown(Key::kKeyA)) {
      h_force = 1.0f;
      current_h_acc -=
        player_data->moveable_entity.horizontal_acceleration_modifier * delta;
    }
    if (game.IsKeyDown(Key::kKeyD)) {
      h_force = -1.0f;
      current_h_acc +=
        player_data->moveable_entity.horizontal_acceleration_modifier * delta;
    }

    if (game.IsKeyDown(Key::kKeySpace)) {
      f32 v_force = 10.0f;
      bool on_ground = OnGround(world, player_data->moveable_entity.position);
      if (on_ground) {
        ForceOnEntity(&player_data->moveable_entity, h_force, v_force);
      }
    }

    MoveEntity(world,
               delta,
               &player_data->moveable_entity,
               current_h_acc,
               current_v_acc);
  }
}

}
