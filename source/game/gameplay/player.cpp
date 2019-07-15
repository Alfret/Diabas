#include "player.hpp"

#include "game/physics/units.hpp"
#include "game/client/game_client.hpp"
#include "game/gameplay/moveable.hpp"
#include <microprofile/microprofile.h>
#include <dutil/stopwatch.hpp>

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
      moveable.input = PlayerInput{};

      if (game.IsKeyDown(Key::kKeyA)) {
        moveable.input.ActionLeft();
      }
      if (game.IsKeyDown(Key::kKeyD)) {
        moveable.input.ActionRight();
      }
      if (game.IsKeyDown(Key::kKeySpace)) {
        moveable.input.ActionJump();
      }

      dutil::FixedTimeUpdate(60, [&]() {
        Packet packet{};
        const auto& packet_handler = network.GetPacketHandler();
        packet_handler.BuildPacketHeader(
          packet, PacketHeaderStaticTypes::kPlayerIncrement);
        auto mw = packet.GetMemoryWriter();
        mw->Write(moveable.ToIncrement());
        mw.Finalize();
        network.PacketBroadcast(packet);
      });
    }
  }
}

}
