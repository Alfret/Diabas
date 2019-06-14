#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include "core/types.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include "network/packet.hpp"
#include "network/common.hpp"

namespace dib
{

class Client : public ISteamNetworkingSocketsCallbacks
{
public:

  Client();

  virtual ~Client() final;

  void Poll();

  /**
   * The result of the connection attempt will be reported later when polling.
   */
  void Connect(const SteamNetworkingIPAddr& address);

  void CloseConnection();

  SendResult SendPacket(const Packet& packet, const SendStrategy send_strategy);

  NetworkState GetNetworkState() const { return network_state_; }

 private:

  void PollSocketStateChanges();

  bool PollIncomingPackets(Packet& packet_out);

  virtual void OnSteamNetConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t* status) override;

private:
  HSteamNetConnection connection_;
  ISteamNetworkingSockets* socket_interface_;
  NetworkState network_state_;
};
}

#endif//CLIENT_HPP_
