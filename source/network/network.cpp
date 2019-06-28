#include "network.hpp"
#include <chrono>
#include <thread>
#include "core/fixed_time_update.hpp"
#include <functional>
#include "game/player_data_storage.hpp"
#include "game/ecs/systems/player_create_system.hpp"
#include "game/ecs/components/uuid_component.hpp"
#include "game/world.hpp"
#include <random>
#include <limits>
#include "game/chat/chat.hpp"

namespace dib {

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
void Network<Side::kServer>::ConnectToServer(u32, u16)
{
  AlfAssert(false, "cannot connect to server from server");
}

template<>
void
Network<Side::kClient>::ConnectToServer(u32 ip, u16 port)
{
  SteamNetworkingIPAddr addr{};
  addr.SetIPv4(ip, port);
  auto client = GetClient();
  client->Connect(addr);
}

template<>
void
Network<Side::kServer>::ConnectToServer(const String&)
{
  AlfAssert(false, "cannot connect to server from server");
}

template<>
void
Network<Side::kClient>::ConnectToServer(const String& addr)
{
  SteamNetworkingIPAddr saddr{};
  saddr.ParseString(addr.GetUTF8());
  auto client = GetClient();
  client->Connect(saddr);
}

template<>
void
Network<Side::kServer>::ConnectToServer(const char8*)
{
  AlfAssert(false, "cannot connect to server from server");
}

template<>
void
Network<Side::kClient>::ConnectToServer(const char8* addr)
{
  SteamNetworkingIPAddr saddr{};
  saddr.ParseString(addr);
  auto client = GetClient();
  client->Connect(saddr);
}

template<>
void
Network<Side::kServer>::Disconnect()
{
  AlfAssert(false, "cannot disconnect on the server");
}

template<>
void
Network<Side::kClient>::Disconnect()
{
  auto client = GetClient();
  client->CloseConnection();
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
Network<Side::kClient>::StartServer()
{
  AlfAssert(false, "cannot start server on client");
}

static void
NetworkInfoCommon(World* world)
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
          auto mw = packet.GetMemoryWriter();
          mw->Write(player_data);
          mw->Write(uuid);
          mw.Finalize();
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

// ============================================================ //
// SetupPacketHandler CLIENT
// ============================================================ //

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
    auto& my_player_data = PlayerDataStorage::Load();
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

    auto mw = player_join_packet.GetMemoryWriter();
    mw->Write(my_player_data);
    mw->Write(uuid);
    mw.Finalize();
    auto client = GetClient();
    client->PacketSend(player_join_packet, SendStrategy::kReliable);
  };
  bool ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kSync, "sync", SyncCb);
  AlfAssert(ok, "could not add packet type sync");

  // ============================================================ //

  const auto ChatCb = [this](const Packet& packet) {
    auto mr = packet.GetMemoryReader();
    auto msg = mr.Read<game::ChatMessage>();
    auto& chat = world_->GetChat();
    chat.ParseMessage(std::move(msg));
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kChat, "chat", ChatCb);
  AlfAssert(ok, "could not add packet type chat");

  // ============================================================ //

  const auto PlayerJoinCb = [this](const Packet& packet) {
    auto mr = packet.GetMemoryReader();
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

  // ============================================================ //

  const auto PlayerLeaveCb = [this](const Packet& packet) {
    auto& registry = world_->GetEntityManager().GetRegistry();
    auto view = registry.view<Uuid>();

    auto mr = packet.GetMemoryReader();
    const auto uuid = mr.Read<Uuid>();

    for (auto entity : view) {
      if (view.get(entity) == uuid) {
        auto player_data = registry.get<PlayerData>(entity);
        DLOG_INFO("{} disconnected", player_data.name);
        registry.destroy(entity);
        break;
      }
    }

    std::string_view _{};
    NetworkInfo(_);
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerLeave, "player leave", PlayerLeaveCb);
  AlfAssert(ok, "could not add packet type player leave");
}

// ============================================================ //
// SetupPacketHandler SERVER
// ============================================================ //

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

  const auto ChatCb = [this](const Packet& packet) {
    auto mr = packet.GetMemoryReader();
    auto msg = mr.Read<game::ChatMessage>();
    if (world_->GetChat().ValidateMessage(msg)) {
      PacketBroadcast(packet);
    } else {
      DLOG_WARNING("client {} attempted to send invalid packet",
                   packet.GetFromConnection());
    }
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kChat, "chat", ChatCb);
  AlfAssert(ok, "could not add packet type chat");

  // ============================================================ //

  const auto PlayerJoinCb = [this](const Packet& packet) {
    DLOG_INFO("player join {}", packet.GetFromConnection());
    auto mr = packet.GetMemoryReader();
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

  // ============================================================ //

  const auto PlayerLeaveCb = [](const Packet&) {
    DLOG_WARNING("Got player leave packet on server");
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerLeave, "player leave", PlayerLeaveCb);
  AlfAssert(ok, "could not add packet type player leave");
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

template<>
ConnectionState
Network<Side::kServer>::GetConnectionState() const
{
  return ConnectionState::kConnected;
}

template<>
ConnectionState
Network<Side::kClient>::GetConnectionState() const
{
  auto client = GetClient();
  return client->GetConnectionState();
}

template<>
u32
Network<Side::kServer>::GetOurEntity() const
{
  AlfAssert(false, "cannot get our entity on server");
  return 0;
}

template<>
u32
Network<Side::kClient>::GetOurEntity() const
{
  auto& registry = world_->GetEntityManager().GetRegistry();
  auto view = registry.view<PlayerConnection>();
  auto client = GetClient();
  for (auto entity : view) {
    if (view.get(entity).connection_id == client->GetConnectionId()) {
      return entity;
    }
  }
  AlfAssert(false, "failed to find our entity");
  return 0;
}

template<>
Uuid
Network<Side::kServer>::GetOurUuid() const
{
  AlfAssert(false, "cannot get our uuid on server");
  return Uuid{};
}

template<>
Uuid
Network<Side::kClient>::GetOurUuid() const
{
  u32 entity = GetOurEntity();
  auto& registry = world_->GetEntityManager().GetRegistry();
  return registry.get<Uuid>(entity);
}

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
