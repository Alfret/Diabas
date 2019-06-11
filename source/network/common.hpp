#ifndef COMMON_HPP_
#define COMMON_HPP_

#include "core/types.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include "network/packet.hpp"

namespace dib
{

enum class SendResult
{
  kSuccess = 0,
  kReconnect,
  kRetry
};

enum class SendStrategy : int
{
  kReliable = k_nSteamNetworkingSend_Reliable,
  kUnreliable = k_nSteamNetworkingSend_Unreliable,

  /**
   * Special use strategies only.
   */
  kReliableNoNagle = k_nSteamNetworkingSend_ReliableNoNagle,
  kUnreliableNoNagle = k_nSteamNetworkingSend_UnreliableNoNagle,
  kUnreliableNoDelay = k_nSteamNetworkingSend_UnreliableNoDelay
};

namespace Common {

SendResult
SendPacket(const Packet& packet, const SendStrategy send_strategy,
           const HSteamNetConnection connection,
           ISteamNetworkingSockets* socket_interface);

}

}

#endif//COMMON_HPP_
