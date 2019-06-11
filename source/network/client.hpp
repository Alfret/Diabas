#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include "core/types.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include "network/packet.hpp"
#include "network/common.hpp"

namespace dib
{

class Client : private ISteamNetworkingSocketsCallbacks
{
public:

  Client();

  virtual ~Client();

  void Poll();

  /**
   * The result of the connection attempt will be reported later when polling.
   */
  void Connect(const SteamNetworkingIPAddr& address);

  void CloseConnection();

  SendResult SendPacket(const Packet& packet, const SendStrategy send_strategy);

 private:

  void PollSocketStateChanges();

  bool PollIncomingPackets(Packet& packet_out);

  virtual void OnSteamNetConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t* status) override;

private:
  HSteamNetConnection connection_;
  ISteamNetworkingSockets* socket_interface_;
};
}

#endif//CLIENT_HPP_
