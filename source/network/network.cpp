#include "network.hpp"
#include <chrono>
#include <thread>
#include "core/fixed_time_update.hpp"
#include <functional>
#include "game/player_data_storage.hpp"
#include <alflib/memory/memory_reader.hpp>
#include <alflib/memory/memory_writer.hpp>
#include "game/ecs/systems/player_connection_system.hpp"
#include "game/world.hpp"

namespace dib {

// template<>
// Network<Side::kClient>::~Network()
// {
//   auto client = GetClient();
//   client->~Client();
//   Network<Side::kClient>::ShutdownNetwork();
// }

// template<>
// Network<Side::kServer>::~Network()
// {
//   auto server = GetServer();
//   server->~Server();
//   Network<Side::kServer>::ShutdownNetwork();
// }

template<>
void
Network<Side::kClient>::ConnectToServer()
{
  SteamNetworkingIPAddr addr{};
  addr.SetIPv4(0x7F000001, kPort);
  auto client = GetClient();
  client->Connect(addr);
}

template<>
void
Network<Side::kServer>::StartServer()
{
  auto server = GetServer();
  server->StartServer(kPort);
}

template<>
void
Network<Side::kClient>::SetupPacketHandler()
{
  const auto SyncCb = [&](const Packet& packet) {
    packet_handler_.OnPacketSync(packet);

    // we are connected, send a player join packet
    Packet player_join_packet{};
    packet_handler_.BuildPacketHeader(player_join_packet,
                                      PacketHeaderStaticTypes::kPlayerJoin);
    auto my_player_data = PlayerDataStorage::Load();
    // TODO dont copy when alflib is updated
    alflib::MemoryWriter mw{};
    mw.Write(my_player_data);
    player_join_packet.SetPayload(mw.GetBuffer().GetData(),
                                  mw.GetBuffer().GetSize());
    auto client = GetClient();
    client->SendPacket(player_join_packet, SendStrategy::kReliable);
  };
  bool ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kSync, "sync", SyncCb);
  AlfAssert(ok, "could not add packet type sync");

  const auto ChatCb = [](const Packet& packet) {
    alflib::String msg = packet.ToString();
    DLOG_RAW("Server: {}\n", msg);
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kChat, "chat", ChatCb);
  AlfAssert(ok, "could not add packet type chat");

  const auto PlayerJoinCb = [](const Packet&) { DLOG_INFO("player join"); };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerJoin, "player join", PlayerJoinCb);
  AlfAssert(ok, "could not add packet type player join");
}

template<>
void
Network<Side::kServer>::SetupPacketHandler()
{
  const auto SyncCb = [](const Packet&) {
    DLOG_INFO("got sync packet, but server does not handle sync packets");
  };
  bool ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kSync, "sync", SyncCb);
  AlfAssert(ok, "could not add packet type sync");

  const auto ChatCb = [](const Packet& packet) {
    alflib::String msg = packet.ToString();
    DLOG_RAW("TODO handle chat packets [{}]\n", msg);
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kChat, "chat", ChatCb);
  AlfAssert(ok, "could not add packet type chat");

  const auto PlayerJoinCb = [this](const Packet& packet) {
    DLOG_INFO("player join");
    // TODO dont copy when alflib is updated
    alflib::Buffer buf(packet.GetPayloadSize(), packet.GetPayload());
    alflib::MemoryReader mr{ buf };
    auto player_data = mr.Read<PlayerData>();
    player_connection_system::Update(world_->GetEntityManager().GetRegistry(),
                                     packet.GetFromConnection(),
                                     player_data);
    // TODO broadcast the new player
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerJoin, "player join", PlayerJoinCb);
  AlfAssert(ok, "could not add packet type player join");
}

template<>
void
Network<Side::kClient>::Update()
{
  // update network
  static bool got_update;
  auto client = GetClient();
  static const auto PollClient =
    std::bind(&Client::Poll, client, std::ref(got_update), std::ref(packet_));
  got_update = false;
  FixedTimeUpdate(kNetTicksPerSec, PollClient);
  if (got_update) {
    packet_handler_.HandlePacket(packet_);
  }
}

template<>
void
Network<Side::kServer>::Update()
{
  // update network
  static bool got_update;
  auto server = GetServer();
  static const auto PollServer =
    std::bind(&Server::Poll, server, std::ref(got_update), std::ref(packet_));
  got_update = false;
  FixedTimeUpdate(kNetTicksPerSec, PollServer);
  if (got_update) {
    packet_handler_.HandlePacket(packet_);
  }
}

template<>
void
Network<Side::kServer>::Broadcast(const Packet& packet) const
{
  auto server = GetServer();
  server->BroadcastPacket(packet, SendStrategy::kReliable);
}

template<>
void
Network<Side::kClient>::Broadcast(const Packet& packet) const
{
  auto client = GetClient();
  const auto res = client->SendPacket(packet, SendStrategy::kReliable);
  if (res != SendResult::kSuccess) {
    // TODO better error handling
    DLOG_WARNING("failed to send packet");
  }
}

template<>
void
Network<Side::kServer>::NetworkInfo([
  [maybe_unused]] const std::string_view message) const
{
  world_->GetEntityManager()
    .GetRegistry()
    .view<PlayerConnection, PlayerData>()
    .each([](const PlayerConnection& player_connection,
             const PlayerData& player_data) {
      DLOG_RAW("\tentity {}, connection {}\n"
               "\t[{}]\n\n",
               player_connection.entity_id,
               player_connection.connection_id,
               player_data.ToString());
    });
}

template<>
void
Network<Side::kClient>::NetworkInfo([
  [maybe_unused]] const std::string_view message) const
{}

template<>
void
Network<Side::kServer>::Broadcast(const std::string_view message) const
{
  auto server = GetServer();
  Packet packet{ message.data(), message.size() };
  packet_handler_.BuildPacketHeader(packet, PacketHeaderStaticTypes::kChat);
  server->BroadcastPacket(packet, SendStrategy::kReliable);
}

template<>
void
Network<Side::kClient>::Broadcast([
  [maybe_unused]] const std::string_view message) const
{}

// ============================================================ //

void
DebugOutputCallback(ESteamNetworkingSocketsDebugOutputType type,
                    const char* msg)
{
  DLOG_VERBOSE("Steam Socket debug output: [{}] [{}]", type, msg);
}

void
SleepAndKill()
{
  // Give the sockets time to finish up, not sure if needed.
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  GameNetworkingSockets_Kill();
}

// ============================================================ //

}
