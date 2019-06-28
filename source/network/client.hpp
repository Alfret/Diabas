#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include "core/types.hpp"
#include "network/common.hpp"
#include "network/packet.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include "network/packet_handler.hpp"
#include "network/connection_state.hpp"

namespace dib {

class World;

class Client : public ISteamNetworkingSocketsCallbacks
{
public:
  Client(PacketHandler* packet_handler, World* world);

  virtual ~Client() final;

  /**
   * Update internal state and check for packets.
   */
  void Poll(bool& got_packet, Packet& packet_out);

  /**
   * The result of the connection attempt will be reported later when polling.
   */
  void Connect(const SteamNetworkingIPAddr& address);

  void CloseConnection();

  SendResult PacketSend(const Packet& packet, const SendStrategy send_strategy);

  ConnectionState GetConnectionState() { return connection_state_; }

  ConnectionId GetConnectionId() const { return connection_; }

private:
  void PollSocketStateChanges();

  bool PollIncomingPackets(Packet& packet_out);

  virtual void OnSteamNetConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t* status) override;

  void SetConnectionState(const ConnectionState connection_state);

private:
  HSteamNetConnection connection_;
  ISteamNetworkingSockets* socket_interface_;
  ConnectionState connection_state_;
  PacketHandler* packet_handler_;
  World* world_;
};
}
#endif // CLIENT_HPP_
