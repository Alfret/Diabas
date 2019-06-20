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
  Server();

  virtual ~Server() final;

  void Poll(bool& got_packet, Packet& packet_out);

  void StartServer(const u16 port);

  void CloseConnection(HSteamNetConnection connection);

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

  void DisconnectClient(const HSteamNetConnection connection);

  std::optional<ClientId> ClientIdFromConnection(
      const HSteamNetConnection connection);

private:
  HSteamListenSocket socket_;
  ISteamNetworkingSockets* socket_interface_;
  std::vector<ClientConnection> clients_{};
  NetworkState network_state_ = NetworkState::kServer;
};
}

#endif // SERVER_HPP_
