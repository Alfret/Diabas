#include "server.hpp"
#include <dlog.hpp>
#include "game/world.hpp"
#include "game/ecs/components/player_connection_component.hpp"
#include "game/ecs/systems/player_connection_system.hpp"

namespace dib {

Server::Server(PacketHandler* packet_handler, World* world)
  : socket_interface_(SteamNetworkingSockets())
  , packet_handler_(packet_handler)
  , world_(world)
{}

Server::~Server()
{
  socket_interface_->CloseListenSocket(socket_);
  for (auto client : clients_) {
    CloseConnection(client);
  }
}

void
Server::Poll(bool& got_packet, Packet& packet_out)
{
  PollSocketStateChanges();
  got_packet = PollIncomingPackets(packet_out);
}

void
Server::StartServer(const u16 port)
{
  SteamNetworkingIPAddr addr{};
  addr.Clear();
  addr.m_port = port;
  socket_ = socket_interface_->CreateListenSocketIP(addr);
  if (socket_ == k_HSteamListenSocket_Invalid) {
    DLOG_ERROR("failed to create a listen socket on port {}", port);
    std::exit(3);
  }
  DLOG_VERBOSE("listening on port {}.", port);
}

void
Server::CloseConnection(HSteamNetConnection connection)
{
  if (connection != k_HSteamNetConnection_Invalid) {
    socket_interface_->CloseConnection(connection, 0, nullptr, false);
  }
}

void
Server::BroadcastPacket(const Packet& packet, const SendStrategy send_strategy)
{
  for (auto client : clients_) {
    Common::SendPacket(packet, send_strategy, client, socket_interface_);
  }
}

SendResult
Server::SendPacket(const Packet& packet,
                   const SendStrategy send_strategy,
                   const HSteamNetConnection connection)
{
  return Common::SendPacket(
    packet, send_strategy, connection, socket_interface_);
}

void
Server::PollSocketStateChanges()
{
  socket_interface_->RunCallbacks(this);
}

bool
Server::PollIncomingPackets(Packet& packet_out)
{
  ISteamNetworkingMessage* msg = nullptr;
  const int msg_count =
    socket_interface_->ReceiveMessagesOnListenSocket(socket_, &msg, 1);

  bool got_packet = false;
  if (msg_count > 0) {
    bool ok =
      packet_out.SetPacket(static_cast<const u8*>(msg->m_pData), msg->m_cbSize);
    if (ok) {
      if (auto it = clients_.find(msg->m_conn); it != clients_.end()) {
        packet_out.SetFromConnection(*it);
        got_packet = true;
      } else {
        DLOG_WARNING("received packet from unknown connection, dropping it");
        DisconnectClient(msg->m_conn);
      }
    } else {
      DLOG_ERROR("could not parse packet, too big [{}/{}]",
                 msg->m_cbSize,
                 packet_out.GetPacketCapacity());
    }

    msg->Release();

  } else if (msg_count < 0) {
    DLOG_WARNING("failed to check for messages, closing connection");
    if (msg != nullptr) {
      DisconnectClient(msg->m_conn);
    }
  }

  return got_packet;
}

void
Server::OnSteamNetConnectionStatusChanged(
  SteamNetConnectionStatusChangedCallback_t* status)
{
  switch (status->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_None: {
      DLOG_VERBOSE("state none");
      break;
    }

    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
      if (status->m_info.m_eState ==
          k_ESteamNetworkingConnectionState_ClosedByPeer) {
        DLOG_INFO("Closed by peer.");
      } else {
        DLOG_WARNING("Problem detected locally.");
      }

      if (status->m_eOldState == k_ESteamNetworkingConnectionState_Connected) {
        clients_.erase(status->m_hConn);

        DLOG_INFO("Connection {}, [{}], disconnected with reason [{}], [{}].",
                  status->m_hConn,
                  status->m_info.m_szConnectionDescription,
                  status->m_info.m_eEndReason,
                  status->m_info.m_szEndDebug);
      } else if (status->m_eOldState !=
                 k_ESteamNetworkingConnectionState_Connecting) {
        DLOG_WARNING("unknown state");
      }

      // TODO announce the disconnect to the rest of the clients?

      CloseConnection(status->m_hConn);
      break;
    }

    case k_ESteamNetworkingConnectionState_Connecting: {
      DLOG_VERBOSE("Connection request from [{}].",
                   status->m_info.m_szConnectionDescription);

      auto [it, ok] = clients_.insert(status->m_hConn);
      if (!ok) {
        DLOG_WARNING("Attempted to add client [{}], but failed.",
                     status->m_hConn);
      }

      if (socket_interface_->AcceptConnection(status->m_hConn) != k_EResultOK) {
        clients_.erase(it);
        CloseConnection(status->m_hConn);
        DLOG_WARNING("Failed to accept a client");
      }

      auto& registry = world_->GetEntityManager().GetRegistry();
      player_connection_system::Update(
        registry, status->m_hConn, ConnectionState::kConnected);

      break;
    }

    case k_ESteamNetworkingConnectionState_Connected: {
      DLOG_VERBOSE("connected");

      // Send a sync packet to the client
      Packet packet{};
      packet_handler_->BuildPacketSync(packet);
      const auto res =
        SendPacket(packet, SendStrategy::kReliable, status->m_hConn);
      if (res != SendResult::kSuccess) {
        DLOG_WARNING("failed to send sync packet to {}, disconnecting them.",
                     status->m_hConn);
        DisconnectClient(status->m_hConn);
      }
      break;
    }

    default: {
      DLOG_WARNING("default ??");
    }
  }
}

void
Server::DisconnectClient(const HSteamNetConnection connection)
{
  if (auto it = clients_.find(connection); it != clients_.end()) {
    DLOG_INFO("Disconnected client [{}].", *it);
    CloseConnection(connection);
    clients_.erase(it);
    auto& registry = world_->GetEntityManager().GetRegistry();
    player_connection_system::Update(
      registry, connection, ConnectionState::kDisconnected);
  }
}
}
