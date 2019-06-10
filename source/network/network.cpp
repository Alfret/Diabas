#include "network.hpp"
#include <dlog.hpp>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include <thread>
#include <chrono>

namespace dib
{

static void
DebugOutputCallback(ESteamNetworkingSocketsDebugOutputType type,
                    const char* msg)
{
  DLOG_VERBOSE("Steam Socket debug output: [{}] [{}]", type, msg);
}

  bool Network::InitNetwork()
{
  SteamDatagramErrMsg errMsg;
  if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
    DLOG_ERROR("GameNetworkingSockets_Init failed.  [{}]", errMsg);
    return false;
  }


    auto detail_level = ESteamNetworkingSocketsDebugOutputType::
      k_ESteamNetworkingSocketsDebugOutputType_Everything;

  SteamNetworkingUtils()->SetDebugOutputFunction(detail_level, DebugOutputCallback);

  return true;
}

void Network::ShutdownNetwork()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  GameNetworkingSockets_Kill();
}

}
