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

    Position a1(100, 25);
    Position b1(100, 20);
    bool did_collide1 = MovePointCollideable(world, a1, b1);

    Position a2(100, 20);
    Position b2(100, 8);
    bool did_collide2 = MovePointCollideable(world, a2, b2);

    Position a3(100, 25);
    Position b3(100, 40);
    bool did_collide3 = MovePointCollideable(world, a3, b3);

    Position a4(100, 100);
    Position b4(200, 2);
    bool did_collide4 = MovePointCollideable(world, a4, b4);

    Position a5(100, 100);
    Position b5(100, 100);
    bool did_collide5 = MovePointCollideable(world, a5, b5);

    Position a6(100, 100);
    Position b6(100.1, 100.1);
    bool did_collide6 = MovePointCollideable(world, a6, b6);

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
