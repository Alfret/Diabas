#include "network.hpp"
#include <chrono>
#include <thread>
#include <functional>
#include "game/client/player_data_storage.hpp"
#include "game/ecs/systems/player_system.hpp"
#include "game/ecs/systems/generic_system.hpp"
#include "game/gameplay/moveable.hpp"
#include "game/gameplay/soul.hpp"
#include "game/npc/npc_id.hpp"
#include "game/npc/npc.hpp"
#include "game/npc/npc_spawn.hpp"
#include "game/world.hpp"
#include <limits>
#include "game/chat/chat.hpp"
#include <dutil/misc.hpp>
#include <dutil/stopwatch.hpp>
#include <microprofile/microprofile.h>
#if !defined(DIB_IS_SERVER)
#include "game/client/render_component.hpp"
#endif

namespace dib {

template<>
void
Network<Side::kServer>::PacketBroadcast(const Packet& packet) const
{
  auto server = GetServer();
  server->PacketBroadcast(packet, SendStrategy::kUnreliableNoNagle);
}

template<>
void
Network<Side::kClient>::PacketBroadcast(const Packet& packet) const
{
  auto client = GetClient();
  const auto res = client->PacketSend(packet, SendStrategy::kUnreliableNoNagle);
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
    packet, SendStrategy::kUnreliableNoNagle, exclude_connection);
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
  world_->OnDisconnect();
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
std::optional<entt::entity>
Network<Side::kServer>::GetOurPlayerEntity() const
{
  AlfAssert(false, "cannot get our player entity on server");
  return std::nullopt;
}

template<>
std::optional<entt::entity>
Network<Side::kClient>::GetOurPlayerEntity() const
{
  auto client = GetClient();
  return client->GetOurPlayerEntity();
}

template<>
void
Network<Side::kServer>::SetOurPlayerEntity(const std::optional<entt::entity>)
{
  AlfAssert(false, "cannot set our player entity on server");
}

template<>
void
Network<Side::kClient>::SetOurPlayerEntity(
  const std::optional<entt::entity> maybe_entity)
{
  auto client = GetClient();
  client->SetOurPlayerEntity(maybe_entity);
}

template<>
std::optional<PlayerData*>
Network<Side::kServer>::GetOurPlayerData() const
{
  AlfAssert(false, "cannot get our PlayerData on server");
  return std::nullopt;
}

template<>
std::optional<PlayerData*>
Network<Side::kClient>::GetOurPlayerData() const
{
  auto& registry = world_->GetEntityManager().GetRegistry();
  if (const auto maybe_entity = GetOurPlayerEntity(); maybe_entity) {
    PlayerData* pd = &registry.get<PlayerData>(*maybe_entity);
    return pd;
  }
  return std::nullopt;
}

template<>
std::optional<entt::entity>
Network<Side::kServer>::EntityFromConnection(const ConnectionId con) const
{
  auto& registry = world_->GetEntityManager().GetRegistry();
  const auto view = registry.view<PlayerData>();
  for (const auto entity : view) {
    if (view.get(entity).connection_id == con) {
      return entity;
    }
  }
  return std::nullopt;
}

template<>
std::optional<entt::entity>
Network<Side::kClient>::EntityFromConnection(const ConnectionId) const
{
  return std::nullopt;
}

template<>
std::optional<SteamNetworkingQuickConnectionStatus>
Network<Side::kServer>::GetConnectionStatus(
  const ConnectionId connection_id) const
{
  auto server = GetServer();
  return server->GetConnectionStatus(connection_id);
}

template<>
std::optional<SteamNetworkingQuickConnectionStatus>
Network<Side::kClient>::GetConnectionStatus([
  [maybe_unused]] const ConnectionId connection_id) const
{
  AlfAssert(false,
            "cannot call GetConnectionStatus(connection_id) from client");
  return std::nullopt;
}

template<>
std::optional<SteamNetworkingQuickConnectionStatus>
Network<Side::kServer>::GetConnectionStatus() const
{
  AlfAssert(false, "cannot call GetConnectionStatus() from server");
  return std::nullopt;
}

template<>
std::optional<SteamNetworkingQuickConnectionStatus>
Network<Side::kClient>::GetConnectionStatus() const
{
  auto client = GetClient();
  return client->GetConnectionStatus();
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
    .view<PlayerData, game::Moveable, game::Soul>()
    .each([&](const PlayerData& player_data,
              const game::Moveable& moveable,
              const game::Soul& soul) {
      if (connection_id != player_data.connection_id) {
        packet.ClearPayload();
        auto mw = packet.GetMemoryWriter();
        AlfAssert(mw->Write(player_data), "write failed");
        AlfAssert(mw->Write(moveable), "write failed");
        AlfAssert(mw->Write(soul), "write failed");
        mw.Finalize();
        auto server = GetServer();
        server->PacketUnicast(
          packet, SendStrategy::kUnreliableNoNagle, connection_id);
      }
    });
}

template<>
void
Network<Side::kClient>::SendPlayerList(const ConnectionId) const
{
  AlfAssert(false, "cannot send player list from client");
}

template <>
void
Network<Side::kServer>::SendNpcList(const ConnectionId connection_id) const
{
  Packet packet{};
  packet_handler_.BuildPacketHeader(packet, PacketHeaderStaticTypes::kNpcSpawn);
  auto& npc_registry = world_->GetNpcRegistry();
  auto& em = world_->GetEntityManager();
  const u32 size = npc_registry.GetNpcs().size();
  packet.SetPacketCapacity(
    256 + size * (sizeof(game::Moveable) + sizeof(game::Soul) +
                  sizeof(game::NpcID) + sizeof(game::NpcType)));

  auto mw = packet.GetMemoryWriter();
  AlfAssert(mw->Write(size), "write failed");

  for (const auto it : npc_registry.GetNpcs()) {
    AlfAssert(mw->Write(it.second->GetType()), "write failed");
    AlfAssert(mw->Write(it.second->GetID()), "write failed");
    AlfAssert(it.second->Store(em, *(*mw)), "write failed");
  }
  mw.Finalize();
  auto server = GetServer();
  server->PacketUnicast(packet, SendStrategy::kUnreliableNoDelay, connection_id);
}

template<>
void
Network<Side::kClient>::SendNpcList(const ConnectionId) const
{
  AlfAssert(false, "cannot send npc list from client");
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
      my_player_data.uuid.GenerateUuid();

      // 2. Insert into ecs system
      auto& registry = world_->GetEntityManager().GetRegistry();
      const auto maybe_entity = system::SafeCreate(registry, my_player_data);
      if (!maybe_entity) {
        DLOG_ERROR("failed to create our PlayerData");
        auto client = GetClient();
        client->CloseConnection();
        return;
      }

      // 3. Make our player entity
      auto client = GetClient();
      client->SetOurPlayerEntity(maybe_entity);

      // 3.1 Set our player RenderComponent
#if !defined(DIB_IS_SERVER)
      auto texture = std::make_shared<graphics::Texture>("Wizard");
      texture->Load(Path{ "./res/entity/wizard.tga" });
      game::RenderComponent renderComponent{ texture };
      system::Assign(registry, *maybe_entity, renderComponent);
#endif

      // 3.2 Set our player Moveable (and Collideable)
      game::Moveable moveable = game::MoveableMakeDefault();
      system::Assign(registry, *maybe_entity, moveable);

      // 3.3 Set our soul
      game::Soul soul{};
      system::Assign(registry, *maybe_entity, soul);

      // 4. Send kPlayerJoin packet
      Packet player_join_packet{};
      packet_handler_.BuildPacketHeader(player_join_packet,
                                        PacketHeaderStaticTypes::kPlayerJoin);
      auto mw = player_join_packet.GetMemoryWriter();
      AlfAssert(mw->Write(my_player_data), "write failed");
      AlfAssert(mw->Write(moveable), "write failed");
      AlfAssert(mw->Write(soul), "write failed");
      mw.Finalize();
      client->PacketSend(player_join_packet, SendStrategy::kUnreliableNoNagle);
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
    // 1. parse the data
    auto mr = packet.GetMemoryReader();
    auto player_data = mr.Read<PlayerData>();
    auto moveable = mr.Read<game::Moveable>();
    auto soul = mr.Read<game::Soul>();
    auto& registry = world_->GetEntityManager().GetRegistry();

    // 2. create entity and fill with PlayerData
    const auto maybe_entity = system::SafeCreate(registry, player_data);
    if (!maybe_entity) {
      DLOG_ERROR("Create on PlayerJoin failed, disconnecting us");
      auto client = GetClient();
      client->CloseConnection();
      return;
    }

    // 3.1 add RenderComponent
#if !defined(DIB_IS_SERVER)
    auto texture = std::make_shared<graphics::Texture>("Pale Man");
    texture->Load(Path{ "./res/entity/pale_man.tga" });
    game::RenderComponent renderComponent{ texture };
    system::Assign(registry, *maybe_entity, renderComponent);
#endif

    // 3.2 add Moveable (and Collideable)
    system::Assign(registry, *maybe_entity, moveable);

    // 3.3 add soul
    system::Assign(registry, *maybe_entity, soul);

    // 4. display all connections
    DLOG_INFO("All active connections:");
    std::string_view _{};
    NetworkInfo(_);
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerJoin, "player join", PlayerJoinCb);
  AlfAssert(ok, "could not add packet type player join");

  // ============================================================ //

  const auto PlayerLeaveCb = [&](const Packet& packet) {
    auto mr = packet.GetMemoryReader();
    const auto uuid = mr.Read<Uuid>();

    auto& registry = world_->GetEntityManager().GetRegistry();
    auto maybe_player_data =
      system::ComponentFromUuid<PlayerData>(registry, uuid);

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

  // ============================================================ //

  const auto PlayerUpdateRejectedCb = [this](const Packet& packet) {
    auto mr = packet.GetMemoryReader();
    auto player_data = mr.Read<PlayerData>();

    DLOG_INFO("player update rejected, replacing our player data with the"
              " the servers version");

    auto& registry = world_->GetEntityManager().GetRegistry();
    const bool ok = system::Replace(registry, player_data);
    if (!ok) {
      DLOG_ERROR("failed to replace PlayerData on PlayerUpdateRejected, "
                 "disconnecting us");
      auto client = GetClient();
      client->CloseConnection();
      return;
    }
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerUpdateRejected,
    "player update rejected",
    PlayerUpdateRejectedCb);
  AlfAssert(ok, "could not add packet type player update rejected");

  // ============================================================ //

  const auto PlayerInputCb = [](const Packet&) {
    DLOG_WARNING("got a PlayerInput packet, but server should "
                 "not send those, ignoring it");
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerInput, "player input", PlayerInputCb);
  AlfAssert(ok, "could not add packet type player input");

  // ============================================================ //

  const auto NpcSpawnCb = [this](const Packet& packet) {
    auto mr = packet.GetMemoryReader();
    auto size = mr.Read<u32>();

    game::NpcType type;
    game::NpcID id;
    game::Npc* npc;
    auto& npc_registry = world_->GetNpcRegistry();
    for (u32 i=0; i<size; i++) {
      type = mr.Read<game::NpcType>();
      id = mr.Read<game::NpcID>();
      npc_registry.Add(world_->GetEntityManager(), id, type);
      npc = npc_registry.Get(id);
      if (npc != nullptr) {
        npc->Load(world_->GetEntityManager(), mr);
      } else {
        DLOG_ERROR("failed to add npc");
        return;
      }
    }
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kNpcSpawn, "npc spawn", NpcSpawnCb);
  AlfAssert(ok, "could not add packet type npc spawn");

  // ============================================================ //

  const auto TickCb = [this](const Packet& packet) {
    auto& registry = world_->GetEntityManager().GetRegistry();
    auto mr = packet.GetMemoryReader();

    { // 1. Players
      const auto size = mr.Read<u32>();
      Uuid uuid;
      game::MoveableIncrement inc;
      game::Soul soul;
      auto view = registry.view<PlayerData, game::Moveable, game::Soul>();
      auto maybe_our_entity = GetOurPlayerEntity();
      if (maybe_our_entity) {
        for (u32 i = 0; i < size; i++) {
          uuid = mr.Read<Uuid>();
          inc = mr.Read<game::MoveableIncrement>();
          soul = mr.Read<game::Soul>();
          for (const auto entity : view) {
            if (view.get<PlayerData>(entity).uuid == uuid) {
              game::ApplyMoveableIncrement(view.get<game::Moveable>(entity),
                                           inc,
                                           *maybe_our_entity == entity);
              view.get<game::Soul>(entity) = soul;
            }
          }
        }
      }
    }

    { // 2. Npc's
      const auto size = mr.Read<u32>();
      game::NpcID id;
      game::Npc* npc;
      for (u32 i=0; i<size; i++) {
        id = mr.Read<game::NpcID>();
        npc = world_->GetNpcRegistry().Get(id);
        if (npc != nullptr) {
          npc->FromIncrement(world_->GetEntityManager(), mr);
        }
      }
    }
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kTick,
    "tick",
    TickCb);
  AlfAssert(ok, "could not add packet type tick");
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
      AlfAssert(mw->Write(msg), "write failed");
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
    auto moveable = mr.Read<game::Moveable>();
    auto soul = mr.Read<game::Soul>();
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
      const auto maybe_entity = system::SafeCreate(registry, player_data);
      if (maybe_entity) {
        registry.assign<game::Moveable>(*maybe_entity, moveable);
        registry.assign<game::Soul>(*maybe_entity, soul);
        server->PacketBroadcastExclude(
          packet, SendStrategy::kUnreliableNoNagle, packet.GetFromConnection());
        SendPlayerList(packet.GetFromConnection());
        SendNpcList(packet.GetFromConnection());
      } else {
        DLOG_WARNING("failed to create PlayerData, disconnecting the "
                     "connection {}",
                     packet.GetFromConnection());
        server->DisconnectConnection(packet.GetFromConnection());
      }
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

  const auto PlayerUpdateCb = [&](const Packet& packet) {
    auto mr = packet.GetMemoryReader();
    auto player_data = mr.Read<PlayerData>();
    player_data.connection_id = packet.GetFromConnection();
    auto& registry = world_->GetEntityManager().GetRegistry();

    // TODO tmp function
    const auto CanPlayerUpdate = [](const PlayerData& player_data) {
      return player_data.name != "dumheter";
    };

    bool legal = true;
    if (const auto maybe_pd = system::PlayerDataFromConnectionId(
          registry, player_data.connection_id);
        maybe_pd) {
      if (**maybe_pd == player_data) {

        const bool accept = CanPlayerUpdate(player_data);
        if (accept) {
          const bool replace_ok = system::Replace(registry, player_data);
          if (!replace_ok) {
            DLOG_WARNING("failed to replace PlayerData for [{}], ignoring",
                         **maybe_pd);
          } else {
            // fill in connection info
            const auto con_status =
              GetConnectionStatus(packet.GetFromConnection());
            if (con_status) {
              player_data.ping = static_cast<u16>(con_status->m_nPing);
              player_data.con_quality_local = static_cast<u8>(
                std::lroundf(dutil::Map(con_status->m_flConnectionQualityLocal,
                                        -1.0f,
                                        1.0f,
                                        0.0f,
                                        255.0f)));
              player_data.con_quality_remote = static_cast<u8>(
                std::lroundf(dutil::Map(con_status->m_flConnectionQualityRemote,
                                        -1.0f,
                                        1.0f,
                                        0.0f,
                                        255.0f)));
            } else {
              DLOG_WARNING("failed to get connection status");
            }

            Packet modified_packet(packet.GetPacketSize());
            modified_packet.SetHeader(*packet.GetHeader());
            auto mw = modified_packet.GetMemoryWriter();
            AlfAssert(mw->Write(player_data), "write failed");
            mw.Finalize();
            PacketBroadcastExclude(modified_packet, player_data.connection_id);
          }
        } else /* !accept */ {

          // Reject the PlayerUpdate
          DLOG_VERBOSE("rejected PlayerUpdate");
          Packet reject_packet{};
          packet_handler_.BuildPacketHeader(
            reject_packet, PacketHeaderStaticTypes::kPlayerUpdateRejected);
          auto mw = reject_packet.GetMemoryWriter();
          AlfAssert(mw->Write(**maybe_pd), "write failed");
          mw.Finalize();
          auto server = GetServer();
          server->PacketUnicast(reject_packet,
                                SendStrategy::kUnreliableNoNagle,
                                packet.GetFromConnection());
        }

      } else {
        DLOG_WARNING("Player [{}] attempted to update someone else's Player"
                     "Data [{}], disconnecting.",
                     **maybe_pd,
                     player_data);
        legal = false;
      }
    } else {
      DLOG_WARNING("Connection {} sent an invalid PlayerUpdate packet,"
                   " disconnecting.",
                   player_data.connection_id);
      legal = false;
    }

    if (!legal) {
      auto server = GetServer();
      server->DisconnectConnection(player_data.connection_id);
    }
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerUpdate, "player update", PlayerUpdateCb);
  AlfAssert(ok, "could not add packet type player update");

  // ============================================================ //

  const auto PlayerUpdateRejectedCb = [this](const Packet& packet) {
    DLOG_WARNING("got a PlayerUpdateRejected packet, but client should "
                 "not send those, disconnecting the client");
    auto server = GetServer();
    server->DisconnectConnection(packet.GetFromConnection());
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerUpdateRejected,
    "player update rejected",
    PlayerUpdateRejectedCb);
  AlfAssert(ok, "could not add packet type player update rejected");

  // ============================================================ //

  const auto PlayerInputCb = [this](const Packet& packet) {
    DLOG_WARNING("got a PlayerInput packet, but client should "
                 "not send those, disconnecting the client");
    auto server = GetServer();
    server->DisconnectConnection(packet.GetFromConnection());
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kPlayerInput, "player input", PlayerInputCb);
  AlfAssert(ok, "could not add packet type player input");

  // ============================================================ //

  const auto NpcSpawnCb = [this](const Packet& packet) {
    // 1. Parse the packet
    auto mr = packet.GetMemoryReader();
    auto type = mr.Read<game::NpcType>();
    game::Position p{mr.Read<decltype(p.x)>(), mr.Read<decltype(p.y)>()};

    // TODO do we want to verify the spawn request somehow?

    // 2. Spawn the npc (and broadcast the new npc).
    game::NpcSpawn<kSide>(*world_, type, p);
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kNpcSpawn, "npc spawn", NpcSpawnCb);
  AlfAssert(ok, "could not add packet type npc spawn");

  // ============================================================ //

  const auto TickCb = [this](const Packet& packet) {
    MICROPROFILE_SCOPEI("network", "PlayerIncrementCb", MP_YELLOW2);
    auto& registry = world_->GetEntityManager().GetRegistry();

    auto mr = packet.GetMemoryReader();
    const auto moveable_increment = mr.Read<game::MoveableIncrement>();

    auto view = registry.view<PlayerData, game::Moveable>();
    bool found = false;
    for (const auto entity : view) {
      if (view.get<PlayerData>(entity).connection_id ==
          packet.GetFromConnection()) {
        found = true;
        // TODO do some cheat checking
        view.get<game::Moveable>(entity).FromIncrement(moveable_increment);
      }
    }
    if (!found) {
      DLOG_WARNING("Got Tick, but player was not found, {}",
                   packet.GetFromConnection());
    }
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kTick,
    "tick",
    TickCb);
  AlfAssert(ok, "could not add packet type tick");
}

template<>
void
Network<Side::kClient>::Update()
{
  MICROPROFILE_SCOPEI("network", "update", MP_YELLOW);

  // update network
  static bool got_update;
  auto client = GetClient();
  static const auto PollClient =
    std::bind(&Client::Poll, client, std::ref(got_update), std::ref(packet_));
  got_update = false;
  dutil::FixedTimeUpdate(kNetTicksPerSec, PollClient);
  while (got_update) {
    bool success = packet_handler_.HandlePacket(packet_);
    if (!success) {
      DLOG_WARNING("Could not handle packet on client");
    }
    PollClient(got_update, packet_);
  }
}

template<>
void
Network<Side::kServer>::Update()
{
  MICROPROFILE_SCOPEI("network", "update", MP_YELLOW);

  // update network
  static bool got_update = false; // needs to be static?
  auto server = GetServer();
  static const auto PollServer =
    std::bind(&Server::Poll, server, std::ref(got_update), std::ref(packet_));
  got_update = false;
  dutil::FixedTimeUpdate(kNetTicksPerSec, PollServer);
  while (got_update) {
    bool success = packet_handler_.HandlePacket(packet_);
    if (!success) {
      DLOG_WARNING("Could not handle packet on server");
    }
    PollServer(got_update, packet_);
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
