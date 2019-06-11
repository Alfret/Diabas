#include "common.hpp"
#include <dlog.hpp>

namespace dib::Common {

SendResult
SendPacket(const Packet& packet, const SendStrategy send_strategy,
           const HSteamNetConnection connection,
           ISteamNetworkingSockets* socket_interface)
{
  const EResult res = socket_interface->SendMessageToConnection(connection, packet.data(), packet.size(), static_cast<int>(send_strategy));

  SendResult result = SendResult::kSuccess;
  if (res != EResult::k_EResultOK) {

    switch (res) {
      case k_EResultInvalidParam:
        DLOG_WARNING(
            "invalid connection handle, or the individual message is too big.");
        result = SendResult::kReconnect;
        if (packet.size() > k_cbMaxSteamNetworkingSocketsMessageSizeSend) {
          DLOG_ERROR(
            "packet size is too big to send, this case is not handled");
        }
        break;

      case k_EResultInvalidState:
        DLOG_WARNING("connection is in an invalid state");
        result = SendResult::kReconnect;
        break;

      case k_EResultNoConnection:
        DLOG_WARNING("attempted to send on a closed connection");
        result = SendResult::kReconnect;
        break;

      case k_EResultIgnored:
        DLOG_WARNING("You used k_nSteamNetworkingSend_NoDelay, and the message "
                     "was dropped because");
        result = SendResult::kRetry;
        break;

      case k_EResultLimitExceeded:
        DLOG_WARNING("there was already too much data queued to be sent.");
        result = SendResult::kRetry;
        break;

      default:
        DLOG_ERROR("default case should never happen");
        result = SendResult::kReconnect;
    }
  }

  return result;
}

}
