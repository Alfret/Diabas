#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "core/types.hpp"
#include "network/common.hpp"
#include "network/packet.hpp"
#include <optional>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include "network/side.hpp"
#include "network/packet_handler.hpp"
#include "network/connection_id.hpp"
#include <tsl/robin_set.h>
#include "network/connection_state.hpp"

namespace dib {

class World;

class Server : public ISteamNetworkingSocketsCallbacks
{
public:
  Server(PacketHandler* packet_handler, World* world);

  virtual ~Server() final;

  void Poll(bool& got_packet, Packet& packet_out);

  void StartServer(const u16 port);

  /**
   * Attempt to close the connection and remove it from our tracked.
   */
  void DisconnectClient(const HSteamNetConnection connection);

  NetworkState GetNetworkState() const { return network_state_; }

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

private:
  HSteamListenSocket socket_;
  ISteamNetworkingSockets* socket_interface_;
  tsl::robin_set<ConnectionId> clients_{};
  NetworkState network_state_ = NetworkState::kServer;
  PacketHandler* packet_handler_;
  World* world_;
};
}

#endif // SERVER_HPP_
