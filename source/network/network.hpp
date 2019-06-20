#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include "core/types.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include "network/side.hpp"
#include "network/client.hpp"
#include "network/common.hpp"
#include "network/server.hpp"
#include <alflib/assert.hpp>
#include <dlog.hpp>


namespace dib{

template <Side side>
class Network
{
  // ============================================================ //
  // Lifetime
  // ============================================================ //
public:
  Network();

  ~Network();

  // ============================================================ //
  // Contants
  // ============================================================ //
public:
  static constexpr u16 kPort = 24812;

  static constexpr s64 kNetTicksPerSec = 32;

  // ============================================================ //
  // Public Methods
  // ============================================================ //
 public:
  void Update();

  void NetworkInfo(const std::string_view message) const;

  void Broadcast(const std::string_view message) const;

  void ConnectToServer();

  void StartServer();

  void SetupPacketHandler();

  // ============================================================ //
  // Private Methods
  // ============================================================ //
private:
  Client* GetClient() const { return static_cast<Client*>(base_); }
  Server* GetServer() const { return static_cast<Server*>(base_); }

  NetworkState GetNetworkState() const;

  /**
   * Call once at startup.
   */
  static bool InitNetwork();

  /**
   * Call once before closing program.
   */
  static void ShutdownNetwork();

  // ============================================================ //
  // Member Variables
  // ============================================================ //
 private:
   /**
    * Packet producer
    * Use GetClient and GetServer to access.
    */
   ISteamNetworkingSocketsCallbacks* base_;

   PacketHandler packet_handler_{};

   Packet packet_{ 10000 };
};

// ============================================================ //
// Template Definition
// ============================================================ //

template<Side side>
void
Network<side>::ConnectToServer()
{
  AlfAssert(false, "attempting to run non specialized code");
}

template<Side side>
void
Network<side>::StartServer()
{
  AlfAssert(false, "attempting to run non specialized code");
}

void
DebugOutputCallback(ESteamNetworkingSocketsDebugOutputType type,
                    const char* msg);

template<Side side>
bool Network<side>::InitNetwork()
{
  SteamDatagramErrMsg errMsg;
  if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
    DLOG_ERROR("GameNetworkingSockets_Init failed.  [{}]", errMsg);
    return false;
  }

  // auto detail_level = ESteamNetworkingSocketsDebugOutputType::
  //   k_ESteamNetworkingSocketsDebugOutputType_Everything;
  auto detail_level = ESteamNetworkingSocketsDebugOutputType::
                      k_ESteamNetworkingSocketsDebugOutputType_Important;

  SteamNetworkingUtils()->SetDebugOutputFunction(detail_level,
                                                 DebugOutputCallback);

  return true;
}

void
SleepAndKill();

template <Side side>
void
Network<side>::ShutdownNetwork()
{
  SleepAndKill();
}
}

#endif // NETWORK_HPP_
