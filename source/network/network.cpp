#include "network.hpp"
#include <chrono>
#include <thread>
#include "core/fixed_time_update.hpp"
#include <functional>
#include "game/player_data_storage.hpp"
#include <alflib/memory/memory_reader.hpp>
#include <alflib/memory/memory_writer.hpp>
#include "game/ecs/systems/player_create_system.hpp"
#include "game/ecs/components/uuid_component.hpp"
#include "game/world.hpp"
#include <random>
#include <limits>

namespace dib {

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

static void
NetworkInfoCommon(game::World* world)
{
  world->GetEntityManager()
    .GetRegistry()
    .view<PlayerConnection, PlayerData, Uuid>()
    .each([](const PlayerConnection& player_connection,
             const PlayerData& player_data,
             const Uuid& uuid) {
      DLOG_RAW("\tentity {}, conn {}, uuid {}\n"
               "\t[{}]\n\n",
               player_connection.entity_id,
               player_connection.connection_id,
               uuid.ToString(),
               player_data.ToString());
    });
}

template<>
void
Network<Side::kServer>::NetworkInfo([
  [maybe_unused]] const std::string_view message) const
{
  NetworkInfoCommon(world_);
}

template<>
void
Network<Side::kClient>::NetworkInfo([
  [maybe_unused]] const std::string_view message) const
{
  NetworkInfoCommon(world_);
}

template<>
void
Network<Side::kServer>::SendPlayerList(const ConnectionId connection_id) const
{
  Packet packet{};
  packet_handler_.BuildPacketHeader(packet,
                                    PacketHeaderStaticTypes::kPlayerJoin);

  world_->GetEntityManager()
    .GetRegistry()
    .view<PlayerConnection, PlayerData, Uuid>()
    .each(
      [&packet, connection_id, this](const PlayerConnection& player_connection,
                                     const PlayerData& player_data,
                                     const Uuid& uuid) {
        if (connection_id != player_connection.connection_id) {
          // TODO dont waste memory when alflib is updated
          alflib::MemoryWriter mw{};
          mw.Write(player_data);
          mw.Write(uuid);
          packet.SetPayload(mw.GetBuffer().GetData(), mw.GetBuffer().GetSize());
          auto server = GetServer();
          server->PacketUnicast(packet, SendStrategy::kReliable, connection_id);
        }
      });
}

template<>
void
Network<Side::kClient>::SendPlayerList(const ConnectionId) const
{
  AlfAssert(false, "cannot send player list from client");
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
    Uuid uuid{ uuids::uuid_system_generator{}() };

    // insert into our ECS system
    auto uuid_ok =
      player_create_system::UpdateUuid(world_->GetEntityManager().GetRegistry(),
                                       packet.GetFromConnection(),
                                       uuid);
    auto player_data_ok = player_create_system::UpdatePlayerData(
      world_->GetEntityManager().GetRegistry(), uuid, my_player_data);
    AlfAssert(player_data_ok && uuid_ok,
              "failed player_data_ok and/or uuid_ok");

    // TODO dont copy when alflib is updated
    alflib::MemoryWriter mw{};
    mw.Write(my_player_data);
    mw.Write(uuid);
    player_join_packet.SetPayload(mw.GetBuffer().GetData(),
                                  mw.GetBuffer().GetSize());
    auto client = GetClient();
    client->PacketSend(player_join_packet, SendStrategy::kReliable);
  };
  bool ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kSync, "sync", SyncCb);
  AlfAssert(ok, "could not add packet type sync");

  // ============================================================ //

  const auto ChatCb = [](const Packet& packet) {
    alflib::String msg = packet.ToString();
    DLOG_RAW("Server: {}\n", msg);
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kChat, "chat", ChatCb);
  AlfAssert(ok, "could not add packet type chat");

  // ============================================================ //

  const auto PlayerJoinCb = [this](const Packet& packet) {
    // TODO dont copy when alflib is updated
    alflib::Buffer buf(packet.GetPayloadSize(), packet.GetPayload());
    alflib::MemoryReader mr{ buf };
    auto player_data = mr.Read<PlayerData>();
    auto uuid = mr.Read<Uuid>();

    // find a unique connection id
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<ConnectionId> dist(
      std::numeric_limits<ConnectionId>::min(),
      std::numeric_limits<ConnectionId>::max());
    ConnectionId cid;
    auto view =
      world_->GetEntityManager().GetRegistry().view<PlayerConnection>();
    for (;;) {
      cid = dist(mt);
      bool found = false;
      for (auto entity : view) {
        if (view.get(entity).connection_id == cid) {
          found = true;
          break;
        }
      }
      if (!found)
        break;
    }

    auto connection_ok = player_create_system::UpdateConnection(
      world_->GetEntityManager().GetRegistry(),
      cid,
      ConnectionState::kConnected);
    auto uuid_ok = player_create_system::UpdateUuid(
      world_->GetEntityManager().GetRegistry(), cid, uuid);
    auto player_data_ok = player_create_system::UpdatePlayerData(
      world_->GetEntityManager().GetRegistry(), uuid, player_data);

    AlfAssert(connection_ok && uuid_ok && player_data_ok,
              "uuid_ok and/or player_data_ok and/or connection_ok failed");

    // display all connections
    DLOG_INFO("All active connections:");
    std::string_view _{};
    NetworkInfo(_);
  };
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

  // ============================================================ //

  const auto ChatCb = [](const Packet& packet) {
    alflib::String msg = packet.ToString();
    DLOG_RAW("TODO handle chat packets [{}]\n", msg);
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kChat, "chat", ChatCb);
  AlfAssert(ok, "could not add packet type chat");

  // ============================================================ //

  const auto PlayerJoinCb = [this](const Packet& packet) {
    DLOG_INFO("player join {}", packet.GetFromConnection());
    // TODO dont copy when alflib is updated
    alflib::Buffer buf(packet.GetPayloadSize(), packet.GetPayload());
    alflib::MemoryReader mr{ buf };
    auto player_data = mr.Read<PlayerData>();
    auto uuid = mr.Read<Uuid>();

    auto uuid_ok =
      player_create_system::UpdateUuid(world_->GetEntityManager().GetRegistry(),
                                       packet.GetFromConnection(),
                                       uuid);
    auto player_data_ok = player_create_system::UpdatePlayerData(
      world_->GetEntityManager().GetRegistry(), uuid, player_data);

    auto server = GetServer();
    if (player_data_ok && uuid_ok) {
      server->PacketBroadcastExclude(
        packet, SendStrategy::kReliable, packet.GetFromConnection());
      SendPlayerList(packet.GetFromConnection());
    } else {
      DLOG_WARNING("failed to update client {}, disconnecting them.",
                   packet.GetFromConnection());
      server->DisconnectConnection(packet.GetFromConnection());
    }
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
    bool success = packet_handler_.HandlePacket(packet_);
    if (!success) {
      DLOG_WARNING("Could not handle packet on client");
    }
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
    bool success = packet_handler_.HandlePacket(packet_);
    if (!success) {
      DLOG_WARNING("Could not handle packet on server");
    }
  }
}

template<>
void
Network<Side::kServer>::PacketBroadcast(const Packet& packet) const
{
  auto server = GetServer();
  server->PacketBroadcast(packet, SendStrategy::kReliable);
}

template<>
void
Network<Side::kClient>::PacketBroadcast(const Packet& packet) const
{
  auto client = GetClient();
  const auto res = client->PacketSend(packet, SendStrategy::kReliable);
  if (res != SendResult::kSuccess) {
    // TODO handle this fail
    DLOG_ERROR("failed to send packet from client, fail not handled");
  }
}

template<>
void
Network<Side::kServer>::PacketBroadcastExclude(
  const Packet& packet,
  const ConnectionId exclude_connection) const
{
  auto server = GetServer();
  server->PacketBroadcastExclude(
    packet, SendStrategy::kReliable, exclude_connection);
}

template<>
void
Network<Side::kClient>::PacketBroadcastExclude(const Packet&,
                                               const ConnectionId) const
{
  AlfAssert(false, "cannot broadcastExclude from client");
}

template<>
void
Network<Side::kServer>::Broadcast(const std::string_view message) const
{
  auto server = GetServer();
  Packet packet{ message.data(), message.size() };
  packet_handler_.BuildPacketHeader(packet, PacketHeaderStaticTypes::kChat);
  server->PacketBroadcast(packet, SendStrategy::kReliable);
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
