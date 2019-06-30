#include "network.hpp"
#include <chrono>
#include <thread>
#include "core/fixed_time_update.hpp"
#include <functional>
#include "game/client/player_data_storage.hpp"
#include "game/ecs/systems/player_system.hpp"
#include "game/ecs/systems/generic_system.hpp"
#include "game/world.hpp"
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

template<>
void
Network<Side::kServer>::NetworkInfo([
  [maybe_unused]] const std::string_view message) const
{
  world_->GetEntityManager().GetRegistry().view<PlayerData>().each(
    [](const PlayerData& player_data) {
      DLOG_RAW("\tConnection: {}\n\tPlayer: [{}]\n",
               player_data.connection_id,
               player_data);
    });
}

template<>
void
Network<Side::kClient>::NetworkInfo([
  [maybe_unused]] const std::string_view message) const
{
  world_->GetEntityManager().GetRegistry().view<PlayerData>().each(
    [](const PlayerData& player_data) {
      DLOG_RAW("\tPlayer: [{}]\n", player_data);
    });
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
    .view<PlayerData>()
    .each(
      [&](const PlayerData& player_data) {
        if (connection_id != player_data.connection_id) {
          packet.Clear();
          auto mw = packet.GetMemoryWriter();
          mw->Write(player_data);
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

    // we are connected
    {
      // 1. Load our PlayerData
      PlayerData my_player_data = PlayerDataStorage::Load();
      my_player_data.connection_id = packet.GetFromConnection();
      my_player_data.uuid.GenerateUuid();

      // 2. Insert into ecs system
      auto& registry = world_->GetEntityManager().GetRegistry();
      const bool ok = system::Create(registry, my_player_data);
      if (!ok) {
        DLOG_ERROR("failed to create our PlayerData");
        auto client = GetClient();
        client->CloseConnection();
        return;
      }

      // 3. Send kPlayerJoin packet
      Packet player_join_packet{};
      packet_handler_.BuildPacketHeader(player_join_packet,
                                        PacketHeaderStaticTypes::kPlayerJoin);
      auto mw = player_join_packet.GetMemoryWriter();
      mw->Write(my_player_data);
      mw.Finalize();
      auto client = GetClient();
      client->PacketSend(player_join_packet, SendStrategy::kReliable);
    }
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
    constexpr ConnectionId kUnusedConnectionId = 0;
    player_data.connection_id = kUnusedConnectionId;
    auto& registry = world_->GetEntityManager().GetRegistry();
    const bool ok = system::Create(registry, player_data);
    if (!ok) {
      DLOG_ERROR("Create on PlayerJoin failed, disconnecting us");
      auto client = GetClient();
      client->CloseConnection();
      return;
    }

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
    auto mr = packet.GetMemoryReader();
    const auto uuid = mr.Read<Uuid>();

    auto& registry = world_->GetEntityManager().GetRegistry();
    auto maybe_player_data = system::ComponentFromUuid<PlayerData>(registry, uuid);

    if (maybe_player_data) {
      DLOG_INFO("[{}] disconnected", **maybe_player_data);
      std::string_view _{};
      NetworkInfo(_);
    } else {
      DLOG_WARNING("unknown connection disconnected");
    }

    system::Delete<PlayerData>(registry, uuid);
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerLeave, "player leave", PlayerLeaveCb);
  AlfAssert(ok, "could not add packet type player leave");

  // ============================================================ //

  const auto PlayerUpdateCb = [this](const Packet& packet) {
    auto mr = packet.GetMemoryReader();
    auto player_data = mr.Read<PlayerData>();

    auto& registry = world_->GetEntityManager().GetRegistry();
    const bool ok = system::Replace(registry, player_data);
    if (!ok) {
      DLOG_WARNING("failed to replace PlayerData on PlayerUpdate, ignoring");
      return;
    }

    // display all connections
    DLOG_INFO("All active connections:");
    std::string_view _{};
    NetworkInfo(_);
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerUpdate, "player update", PlayerUpdateCb);
  AlfAssert(ok, "could not add packet type player update");
}

// ============================================================ //
// SetupPacketHandler SERVER
// ============================================================ //

template<>
void
Network<Side::kServer>::SetupPacketHandler()
{
  const auto SyncCb = [](const Packet&) {
    DLOG_WARNING("got sync packet, but server does not handle sync packets");
  };
  bool ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kSync, "sync", SyncCb);
  AlfAssert(ok, "could not add packet type sync");

  // ============================================================ //

  const auto ChatCb = [&](const Packet& packet) {
    auto mr = packet.GetMemoryReader();
    auto msg = mr.Read<game::ChatMessage>();
    if (world_->GetChat().ValidateMessage(msg)) {
      world_->GetChat().FillFromTo(msg);

      Packet new_packet{};
      new_packet.SetHeader(*packet.GetHeader());
      auto mw = new_packet.GetMemoryWriter();
      mw->Write(msg);
      mw.Finalize();
      PacketBroadcast(new_packet);
    } else {
      DLOG_WARNING("[{}] attempted to send invalid chat message",
                   packet.GetFromConnection());
    }
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kChat, "chat", ChatCb);
  AlfAssert(ok, "could not add packet type chat");

  // ============================================================ //

  const auto PlayerJoinCb = [&](const Packet& packet) {
    auto& registry = world_->GetEntityManager().GetRegistry();
    auto server = GetServer();

    // Read the PlayerData
    auto mr = packet.GetMemoryReader();
    auto player_data = mr.Read<PlayerData>();
    player_data.connection_id = packet.GetFromConnection();

    // Does the player exist
    auto maybe_player_data =
      system::PlayerDataFromConnectionId(registry, packet.GetFromConnection());

    // Do we have the same Uuid, but from another connection?
    if (maybe_player_data && player_data != **maybe_player_data) {
        DLOG_WARNING("player [{}] tried to update someone else's PlayerData",
                     **maybe_player_data);
        // TODO could the existing player_data be wrong in this case?
        // Maybe disconnect both, or something.. See if player_data connection
        // is actually active?
        server->DisconnectConnection(packet.GetFromConnection());
    } else {
      DLOG_INFO("player join [{}]", player_data);
      const bool ok = system::Create(registry, player_data);
      if (!ok) {
        DLOG_WARNING("failed to create PlayerData, disconnecting the "
                     "connection {}", packet.GetFromConnection());
        server->DisconnectConnection(packet.GetFromConnection());
      }
      server->PacketBroadcastExclude(
        packet, SendStrategy::kReliable, packet.GetFromConnection());
      SendPlayerList(packet.GetFromConnection());
    }
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerJoin, "player join", PlayerJoinCb);
  AlfAssert(ok, "could not add packet type player join");

  // ============================================================ //

  const auto PlayerLeaveCb = [](const Packet&) {
    DLOG_WARNING("Got player leave packet on server, but server does not "
                 "handle those");
    // TODO if a player disconnects while "in combat", have an instance of the
    // player be left inside, to avoid player cheesing combat mechanics
    // by disconnecting.
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerLeave, "player leave", PlayerLeaveCb);
  AlfAssert(ok, "could not add packet type player leave");

  // ============================================================ //

  const auto PlayerUpdateCb = [this](const Packet& packet) {
    auto mr = packet.GetMemoryReader();
    auto player_data = mr.Read<PlayerData>();
    player_data.connection_id = packet.GetFromConnection();
    auto& registry = world_->GetEntityManager().GetRegistry();

    // check if valid update
    bool ok = true;
    if (auto maybe_pd = system::PlayerDataFromConnectionId(registry, player_data.connection_id);
        maybe_pd) {
      if (**maybe_pd == player_data) {
        const bool uok = system::Replace(registry, player_data);
        if (!uok) {
          DLOG_WARNING("failed to replace PlayerData for [{}], ignoring",
                       **maybe_pd);
        } else {
          PacketBroadcastExclude(packet, player_data.connection_id);
        }
      } else {
        DLOG_WARNING("Player [{}] attempted to update someone else's Player"
                     "Data [{}], disconnecting.",
                     **maybe_pd, player_data);
        ok = false;
      }
    } else {
      DLOG_WARNING("Connection {} sent an invalid PlayerUpdate packet,"
                   " disconnecting.", player_data.connection_id);
      ok = false;
    }

    if (!ok) {
      auto server = GetServer();
      server->DisconnectConnection(player_data.connection_id);
    }
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerUpdate, "player update", PlayerUpdateCb);
  AlfAssert(ok, "could not add packet type player update");
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
  game::ChatMessage msg{};
  msg.type = game::ChatType::kServer;
  msg.msg = String(message.data());
  world_->GetChat().FillFromTo(msg);
  if (!world_->GetChat().SendMessage(msg)) {
    DLOG_WARNING("failed to broadcast a server chat message");
  }
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
  const auto view = registry.view<PlayerData>();
  const auto client = GetClient();
  for (const auto entity : view) {
    if (view.get(entity).connection_id == client->GetConnectionId()) {
      return entity;
    }
  }
  AlfAssert(false, "failed to find our entity");
  return 0;
}

template<>
const Uuid*
Network<Side::kServer>::GetOurUuid() const
{
  AlfAssert(false, "cannot get our uuid on server");
  return nullptr;
}

template<>
const Uuid*
Network<Side::kClient>::GetOurUuid() const
{
  const u32 entity = GetOurEntity();
  const auto& registry = world_->GetEntityManager().GetRegistry();
  return &registry.get<PlayerData>(entity).uuid;
}

template<>
const PlayerData*
Network<Side::kServer>::GetOurPlayerData() const
{
  AlfAssert(false, "cannot get our PlayerData on server");
  return nullptr;
}

template <>
const PlayerData*
Network<Side::kClient>::GetOurPlayerData() const
{
  const u32 entity = GetOurEntity();
  const auto& registry = world_->GetEntityManager().GetRegistry();
  return &registry.get<PlayerData>(entity);
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
