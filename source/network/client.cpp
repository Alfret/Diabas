#include "client.hpp"
#include <dlog.hpp>

namespace dib {

Client::Client()
  : connection_(k_HSteamNetConnection_Invalid)
  , socket_interface_(SteamNetworkingSockets())
  , network_state_(NetworkState::kClientOffline)
{}

Client::~Client()
{
  CloseConnection();
}

void
Client::Poll()
{
  PollSocketStateChanges();
  Packet packet_out{};
  PollIncomingPackets(packet_out);
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
    socket_interface_->CloseConnection(connection_, 0, nullptr, false);
    connection_ = k_HSteamNetConnection_Invalid;
  }
  network_state_ = NetworkState::kClientOffline;
}

SendResult
Client::SendPacket(const Packet& packet, const SendStrategy send_strategy)
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

  bool retval = false;
  if (msg_count > 0) {
    packet_out.resize(msg->m_cbSize);
    memcpy(packet_out.data(), msg->m_pData, msg->m_cbSize);

    msg->Release();
    retval = true;

    // TODO not this
    std::string msg{ packet_out.begin(), packet_out.end() };
    DLOG_RAW("Server: {}\n", msg);

  } else if (msg_count < 0) {
    if (network_state_ == NetworkState::kClientOnline) {
      DLOG_ERROR("failed to check for messages");
      // TODO close connection and retry to connect?
    }
  }

  return retval;
}

void
Client::OnSteamNetConnectionStatusChanged(
  SteamNetConnectionStatusChangedCallback_t* status)
{
  switch (status->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_None: {
      DLOG_VERBOSE("state none");
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
      network_state_ = NetworkState::kClientOnline;
      break;
    }

    default: {
      DLOG_VERBOSE("default");
    }
  }
}
}
