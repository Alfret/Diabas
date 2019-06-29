#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include "core/types.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include "network/side.hpp"
#include "network/client.hpp"
#include "network/common.hpp"
#include "network/server.hpp"
#include <alflib/core/assert.hpp>
#include <dlog.hpp>
#include <functional>
#include "network/connection_state.hpp"
#include "core/macros.hpp"
#include "core/uuid.hpp"

// ========================================================================== //
// Forward Declarations
// ========================================================================== //

namespace dib::game {
DIB_FORWARD_DECLARE_CLASS(World);
}

// ========================================================================== //
// Network
// ========================================================================== //

namespace dib {

template<Side side>
class Network
{
  // ============================================================ //
  // Lifetime
  // ============================================================ //
public:
  Network(game::World* world);

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

  /**
   * Server: Broadcast the packet to all active connections.
   * Client: Unicast the packet to the server.
   */
  void PacketBroadcast(const Packet& packet) const;

  /**
   * Server: Broadcast to all active connections but the excluded one.
   * Client: assert(false)
   */
  void PacketBroadcastExclude(const Packet& packet,
                              const ConnectionId exclude_connection) const;

  void ConnectToServer(u32 ip, u16 port);

  void ConnectToServer(const String& addr);
  void ConnectToServer(const char8* addr);

  void Disconnect();

  void StartServer();

  /**
   * Server: always returns kConnected
   */
  ConnectionState GetConnectionState() const;

  u32 GetOurEntity() const;

  Uuid GetOurUuid() const;

  // ============================================================ //
  // TMP
  // ============================================================ //
  void NetworkInfo(const std::string_view message) const;

  void Broadcast(const std::string_view message) const;

  PacketHandler& GetPacketHandler() { return packet_handler_; }

  // ============================================================ //
  // Private Methods
  // ============================================================ //
private:
  void SetupPacketHandler();

  Client* GetClient() const { return static_cast<Client*>(base_); }
  Server* GetServer() const { return static_cast<Server*>(base_); }

  /**
   * Call once at startup.
   */
  static bool InitNetwork();

  /**
   * Call once before closing program.
   */
  static void ShutdownNetwork();

  void SendPlayerList(const ConnectionId connection_id) const;

  // void OnConnectionChange(Connection)

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

  game::World* world_;
};

// ============================================================ //
// Template Definition
// ============================================================ //

template<Side side>
Network<side>::Network(game::World* world)
  : world_(world)
{
  if (!Network<side>::InitNetwork()) {
    DLOG_ERROR("Failed to init network.");
    std::exit(1);
  }
  SetupPacketHandler();
  if constexpr (side == Side::kServer) {
    base_ = new Server(&packet_handler_, world);
    StartServer();
  } else {
    base_ = new Client(&packet_handler_, world);
  }
}

template<Side side>
Network<side>::~Network()
{
  if constexpr (side == Side::kServer) {
    auto server = GetServer();
    server->~Server();
  } else {
    auto client = GetClient();
    client->~Client();
  }
  Network<side>::ShutdownNetwork();
}

void
DebugOutputCallback(ESteamNetworkingSocketsDebugOutputType type,
                    const char* msg);

template<Side side>
bool
Network<side>::InitNetwork()
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

template<Side side>
void
Network<side>::ShutdownNetwork()
{
  SleepAndKill();
}
}

#endif // NETWORK_HPP_
