#include "client.hpp"
#include <dlog.hpp>
#include "game/world.hpp"
#include "game/ecs/components/player_connection_component.hpp"
#include "game/ecs/systems/player_create_system.hpp"

namespace dib {

Client::Client(PacketHandler* packet_handler, game::World* world)
  : connection_(k_HSteamNetConnection_Invalid)
  , socket_interface_(SteamNetworkingSockets())
  , connection_state_(ConnectionState::kDisconnected)
  , packet_handler_(packet_handler)
  , world_(world)
{}

Client::~Client()
{
  CloseConnection();
}

void
Client::Poll(bool& got_packet, Packet& packet_out)
{
  PollSocketStateChanges();
  got_packet = PollIncomingPackets(packet_out);
}

void
Client::Connect(const SteamNetworkingIPAddr& address)
{
  connection_ = socket_interface_->ConnectByIPAddress(address);
  if (connection_ == k_HSteamNetConnection_Invalid) {
    DLOG_WARNING("failed to connect");
  }
}

void
Client::CloseConnection()
{
  if (connection_ != k_HSteamNetConnection_Invalid) {
    SetConnectionState(ConnectionState::kDisconnected);
    socket_interface_->CloseConnection(connection_, 0, nullptr, false);
    connection_ = k_HSteamNetConnection_Invalid;
  }

  // clear all player entities
  auto& registry = world_->GetEntityManager().GetRegistry();
  auto view = registry.view<PlayerConnection>();
  for (auto entity : view) {
    registry.destroy(entity);
  }
}

SendResult
Client::PacketSend(const Packet& packet, const SendStrategy send_strategy)
{
  return Common::SendPacket(
    packet, send_strategy, connection_, socket_interface_);
}

void
Client::PollSocketStateChanges()
{
  socket_interface_->RunCallbacks(this);
}

bool
Client::PollIncomingPackets(Packet& packet_out)
{
  ISteamNetworkingMessage* msg = nullptr;
  const int msg_count =
    socket_interface_->ReceiveMessagesOnConnection(connection_, &msg, 1);

  bool got_packet = false;
  if (msg_count > 0) {
    bool ok =
      packet_out.SetPacket(static_cast<const u8*>(msg->m_pData), msg->m_cbSize);
    if (ok) {
      packet_out.SetFromConnection(msg->m_conn);
      got_packet = true;
    } else {
      DLOG_ERROR("could not parse packet, too big [{}/{}]",
                 msg->m_cbSize,
                 packet_out.GetPacketCapacity());
    }
    msg->Release();

  } else if (msg_count < 0) {
    if (connection_state_ != ConnectionState::kDisconnected) {
      DLOG_VERBOSE("failed to check for messages, disconnecting");
      CloseConnection();
    }
  }

  return got_packet;
}

void
Client::OnSteamNetConnectionStatusChanged(
  SteamNetConnectionStatusChangedCallback_t* status)
{
  switch (status->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_None: {
      // DLOG_VERBOSE("state none");
      break;
    }

    case k_ESteamNetworkingConnectionState_ClosedByPeer: {
      if (status->m_eOldState == k_ESteamNetworkingConnectionState_Connecting) {
        DLOG_WARNING("Connection could not be established.");
      } else {
        DLOG_WARNING("connection closed by peer");
      }

      // cleanup connection
      CloseConnection();
      break;
    }

    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
      DLOG_WARNING("problem detected locally");

      // cleanup connection
      CloseConnection();
      break;
    }

    case k_ESteamNetworkingConnectionState_Connecting: {
      DLOG_VERBOSE("connecting");
      break;
    }

    case k_ESteamNetworkingConnectionState_Connected: {
      DLOG_INFO("connected");
      SetConnectionState(ConnectionState::kConnected);
      break;
    }

    default: {
      DLOG_WARNING("default ??");
    }
  }
}

void
Client::SetConnectionState(const ConnectionState connection_state)
{
  connection_state_ = connection_state;
  auto& registry = world_->GetEntityManager().GetRegistry();
  player_create_system::UpdateConnection(
    registry, connection_, connection_state);
}
}
