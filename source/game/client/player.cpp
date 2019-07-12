#include "player.hpp"

#include "game/physics/units.hpp"
#include "game/client/game_client.hpp"
#include "game/physics/moveable.hpp"
#include <microprofile/microprofile.h>
#include <dutil/stopwatch.hpp>

namespace dib::game {

void
Player::Update(GameClient& game, const f64 delta)
{
  if constexpr (kSide == Side::kClient) {
      MICROPROFILE_SCOPEI("player", "update moveable", MP_GREEN);

      auto& world = game.GetWorld();
      auto& network = world.GetNetwork();
      if (auto maybe_entity = network.GetOurPlayerEntity(); maybe_entity) {
        auto& registry = world.GetEntityManager().GetRegistry();
        Moveable& moveable = registry.get<Moveable>(*maybe_entity);
        moveable.input = PlayerInput{};

        bool action = false;
        if (game.IsKeyDown(Key::kKeyA)) {
          action = true;
          moveable.input.ActionLeft();

        }
        if (game.IsKeyDown(Key::kKeyD)) {
          action = true;
          moveable.input.ActionRight();
        }

        if (game.IsKeyDown(Key::kKeySpace)) {
          action = true;
          moveable.input.ActionJump();
        }

        if (action) {
          //UpdateMoveable(world, delta, moveable);
        }

        dutil::FixedTimeUpdate(60, [&]() {
          Packet packet{};
          auto& packet_handler = network.GetPacketHandler();
          packet_handler.BuildPacketHeader(
            packet, PacketHeaderStaticTypes::kPlayerInput);
          auto mw = packet.GetMemoryWriter();
          mw->Write(moveable.input);
          mw.Finalize();
          network.PacketBroadcast(packet);
        });
      }
    }
}

}
