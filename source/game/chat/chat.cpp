#include "chat.hpp"
#include <alflib/core/assert.hpp>
#include "game/world.hpp"
#include "core/uuid.hpp"
#include "game/ecs/components/player_data_component.hpp"
#include "game/ecs/systems/generic_system.hpp"
#include "network/side.hpp"

namespace dib::game {

Chat::Chat(World* world)
  : world_(world)
{}

bool
Chat::SendMessage(ChatMessage msg) const
{
  auto& network = world_->GetNetwork();

  if (!ValidateMessage(msg)) {
    return false;
  }

  Packet packet{};
  auto& packet_handler = network.GetPacketHandler();
  packet_handler.BuildPacketHeader(packet, PacketHeaderStaticTypes::kChat);
  auto mw = packet.GetMemoryWriter();
  mw->Write(msg);
  mw.Finalize();
  network.PacketBroadcast(packet);

  return true;
}

void
Chat::ParseMessage(ChatMessage&& msg)
{
  auto res = messages_.Push(std::move(msg));
  if (res != dutil::QueueResult::kSuccess) {
    ChatMessage _{};
    res = messages_.Pop(_);
    AlfAssert(res == dutil::QueueResult::kSuccess, "failed to pop");
    res = messages_.Push(std::move(msg));
    AlfAssert(res == dutil::QueueResult::kSuccess, "failed the second push");
  }
  Debug();
}

bool
Chat::ValidateMessage(const ChatMessage& msg) const
{
  auto& registry = world_->GetEntityManager().GetRegistry();
  auto& network = world_->GetNetwork();

  // verify the data in chat message
  {
    if (msg.msg.GetSize() < 1) {
      DLOG_WARNING("attemted to sent an empty chat message");
      return false;
    }

    if (const u32 v = static_cast<u32>(msg.type);
        v > static_cast<u32>(ChatType::kEvent)) {
      DLOG_WARNING("attemted to send a message of unknown type");
      return false;
    }

    if constexpr (kSide == Side::kClient) {
      if (msg.type == ChatType::kEvent || msg.type == ChatType::kServer) {
        DLOG_WARNING("Client attempted to send a privileged chat message");
        return false;
      }

      if (network.GetOurPlayerData() &&
          (*network.GetOurPlayerData())->uuid != msg.uuid_from) {
        DLOG_WARNING("attempted to sent a message that was not from us");
        return false;
      }

      if (msg.type == ChatType::kWhisper) {
        auto view = registry.view<Uuid>();
        bool found = false;
        for (auto entity : view) {
          if (view.get(entity) == msg.uuid_to) {
            found = true;
            break;
          }
        }

        if (!found) {
          DLOG_WARNING("attemted to send a whisper, but invalid to uuid");
          return false;
        }
      }
    }
  }
  return true;
}

void
Chat::FillFromTo(ChatMessage& msg) const
{
  if constexpr (kSide == Side::kServer) {
    auto& registry = world_->GetEntityManager().GetRegistry();

    if (msg.type == game::ChatType::kSay ||
        msg.type == game::ChatType::kWhisper) {
      auto maybe_pd =
        system::ComponentFromUuid<PlayerData>(registry, msg.uuid_from);
      if (maybe_pd) {
        msg.from = (*maybe_pd)->name;
      }
      // TODO handle whisper
    } else if (msg.type == game::ChatType::kServer) {
      msg.from = "SERVER";
    } else if (msg.type == game::ChatType::kEvent) {
      // TODO change this
      msg.from = "EVENT";
    }
  } else {
    AlfAssert(false, "cannot use FillFromTo from client side");
  }
}

void
Chat::Debug()
{
  debug_ = "";
  for (auto message : messages_) {
    if (message.from.GetSize() > 0) {
      debug_ += message.from + ": ";
    }
    debug_ += message.msg + "\n\n";
  }
}

}
