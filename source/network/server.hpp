#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include "core/types.hpp"
#include <vector>
#include "network/common.hpp"
#include "network/packet.hpp"
#include <optional>

namespace dib
{

struct ClientId
{
  u32 id;
};

// ============================================================ //

class ClientIdGenerator
{
public:
  static ClientId Next() {
    static ClientIdGenerator gen{};
    const ClientId value = gen.client_id_;
    ++gen.client_id_.id;
    return value;
  }
 private:
  ClientId client_id_{0};
};

// ============================================================ //

struct ClientConnection {
  ClientId client_id;
  HSteamNetConnection connection;
};

// ============================================================ //

class Server : private ISteamNetworkingSocketsCallbacks
{
public:
  Server();

  ~Server();

  void Poll();

  void StartServer(const u16 port);

  void CloseConnection(HSteamNetConnection connection);



private:

  SendResult SendPacket(const Packet& packet, const SendStrategy send_strategy,
                        const HSteamNetConnection connection);

  void PollSocketStateChanges();

  void PollIncomingPackets();

  virtual void OnSteamNetConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t* status) override;

  void DisconnectClient(const HSteamNetConnection connection);

  std::optional<ClientId> ClientIdFromConnection(const HSteamNetConnection connection);

private:
  HSteamListenSocket socket_;
  ISteamNetworkingSockets* socket_interface_;
  std::vector<ClientConnection> clients_{};
  Packet packet_{};
};
}

#endif//SERVER_HPP_
