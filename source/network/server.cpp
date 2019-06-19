#include "server.hpp"
#include <dlog.hpp>

namespace dib {

Server::Server()
  : socket_interface_(SteamNetworkingSockets())
{
  packet_.reserve(10000);
}

Server::~Server()
{
  socket_interface_->CloseListenSocket(socket_);
  for (auto client : clients_) {
    CloseConnection(client.connection);
  }
}

void
Server::Poll()
{
  PollSocketStateChanges();
  PollIncomingPackets();
}

void
Server::StartServer(const u16 port)
{
  SteamNetworkingIPAddr addr{};
  addr.Clear();
  addr.m_port = port;
  socket_ = socket_interface_->CreateListenSocketIP(addr);
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
Server::NetworkInfo() const
{
  DLOG_RAW("{:=^30}\n", " Network Connections ");
  if (clients_.size() > 0) {
    for (const auto client : clients_) {
      DLOG_RAW("\tid: {}\n", client.client_id.id);
    }
  } else {
    DLOG_RAW("\tnone\n");
  }

  DLOG_RAW("\n");
}

void
Server::BroadcastPacket(const Packet& packet,
                   const SendStrategy send_strategy)
{
  for (auto client : clients_) {
    Common::SendPacket(packet, send_strategy, client.connection, socket_interface_);
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

void
Server::PollIncomingPackets()
{
  ISteamNetworkingMessage* msg = nullptr;
  const int msg_count =
    socket_interface_->ReceiveMessagesOnListenSocket(socket_, &msg, 1);

  if (msg_count > 0) {
    packet_.resize(msg->m_cbSize);
    memcpy(packet_.data(), msg->m_pData, msg->m_cbSize);

    msg->Release();

    auto maybe_clientid = ClientIdFromConnection(msg->m_conn);
    if (!maybe_clientid) {
      DLOG_WARNING("received packet from unknown connection, dropping it");
      DisconnectClient(msg->m_conn);
    }

    if (packet_.size() < 100) {
      std::string str{ packet_.begin(), packet_.end() };
      DLOG_INFO("TODO handle packet [{}] from [{}]", str, maybe_clientid->id);
    } else {
      DLOG_INFO("TODO handle packet from [{}]", maybe_clientid->id);
    }

  } else if (msg_count < 0) {
    DLOG_WARNING("failed to check for messages, closing connection");
    DisconnectClient(msg->m_conn);
  }
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
        ClientId removed_id{};
        for (auto it = clients_.begin(); it < clients_.end(); it++) {
          if (it->connection == status->m_hConn) {
            removed_id = it->client_id;
            clients_.erase(it);
            break;
          }
        }

        DLOG_INFO(
          "Connection [{}] on [{}], disconnected with reason [{}], [{}].",
          removed_id.id,
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

      bool already_exists = false;
      ClientId existing_id{};
      for (auto client : clients_) {
        if (client.connection == status->m_hConn) {
          already_exists = true;
          existing_id = client.client_id;
          break;
        }
      }
      if (already_exists) {
        DLOG_WARNING(
          "Client [{}] attempted to connect while already being connected.",
          existing_id.id);
        break;
      }

      if (socket_interface_->AcceptConnection(status->m_hConn) != k_EResultOK) {
        CloseConnection(status->m_hConn);
        DLOG_WARNING("Failed to accept a client");
      }

      clients_.push_back({ ClientIdGenerator::Next(), status->m_hConn });

      // TODO do a handshake with the client, and maybe send some welcome
      // information

      break;
    }

    case k_ESteamNetworkingConnectionState_Connected: {
      DLOG_VERBOSE("connected");
      break;
    }

    default: {
      DLOG_VERBOSE("default");
    }
  }
}

void
Server::DisconnectClient(const HSteamNetConnection connection)
{
  for (auto it = clients_.begin(); it < clients_.end(); it++) {
    if (it->connection == connection) {
      DLOG_INFO("Disconnected client [{}].", it->client_id.id);
      clients_.erase(it);
      break;
    }
  }
}

std::optional<ClientId>
Server::ClientIdFromConnection(const HSteamNetConnection connection)
{
  for (auto client : clients_) {
    if (client.connection == connection) {
      return std::optional<ClientId>(client.client_id);
    }
  }
  return std::nullopt;
}

}
