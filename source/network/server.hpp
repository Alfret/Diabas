#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "core/types.hpp"
#include "network/common.hpp"
#include "network/packet.hpp"
#include <optional>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include "network/side.hpp"
#include <vector>
#include "network/packet_handler.hpp"

namespace dib {

struct ClientId
{
  u32 id;
};

// ============================================================ //

class ClientIdGenerator
{
public:
  static ClientId Next()
  {
    static ClientIdGenerator gen{};
    const ClientId value = gen.client_id_;
    ++gen.client_id_.id;
    return value;
  }

private:
  ClientId client_id_{ 0 };
};

// ============================================================ //

struct ClientConnection
{
  ClientId client_id;
  HSteamNetConnection connection;
};

// ============================================================ //

class Server : public ISteamNetworkingSocketsCallbacks
{
public:
  Server(PacketHandler* packet_handler);

  virtual ~Server() final;

  void Poll(bool& got_packet, Packet& packet_out);

  void StartServer(const u16 port);

  /**
   * Attempt to close the connection and remove it from our tracked.
   */
  void DisconnectClient(const HSteamNetConnection connection);

  NetworkState GetNetworkState() const { return network_state_; }

  void NetworkInfo() const;

  void BroadcastPacket(const Packet& packet, const SendStrategy send_strategy);

  SendResult SendPacket(const Packet& packet,
                        const SendStrategy send_strategy,
                        const HSteamNetConnection connection);

private:
  void PollSocketStateChanges();

  bool PollIncomingPackets(Packet& packet_out);

  virtual void OnSteamNetConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t* status) override;

  /**
   * Let steam sockets close the connection.
   */
  void CloseConnection(HSteamNetConnection connection);

  std::optional<ClientId> ClientIdFromConnection(
      const HSteamNetConnection connection);

  auto ClientIteratorFromConnection(const HSteamNetConnection connection) const;

private:
  HSteamListenSocket socket_;
  ISteamNetworkingSockets* socket_interface_;
  std::vector<ClientConnection> clients_{};
  NetworkState network_state_ = NetworkState::kServer;
  PacketHandler* packet_handler_;
};
}

#endif // SERVER_HPP_
