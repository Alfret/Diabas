#include "world.hpp"
#include "core/fixed_time_update.hpp"
#include <functional>

namespace dib {

/**
 * Note, ordering in this file is weird because of template specialization
 * initialization requirements.
 */

template<>
void
World<Side::kClient>::Update()
{
  auto client = GetClient();
  static const auto callback_fn = std::bind(&Client::Poll, client);

  FixedTimeUpdate(kNetTicksPerSec, callback_fn);
}

template<>
void
World<Side::kServer>::Update()
{
  auto server = GetServer();
  static const auto callback_fn = std::bind(&Server::Poll, server);

  FixedTimeUpdate(kNetTicksPerSec, callback_fn);
}

template<>
void
World<Side::kClient>::ConnectToServer()
{
  SteamNetworkingIPAddr addr{};
  addr.SetIPv4(0x7F000001, kPort);
  auto client = GetClient();
  client->Connect(addr);
}

template<>
void
World<Side::kServer>::StartServer()
{
  auto server = GetServer();
  server->StartServer(kPort);
}

template<>
World<Side::kClient>::World()
  : base_(new Client())
{
  ConnectToServer();
}

template<>
World<Side::kServer>::World()
  : base_(new Server())
{
  StartServer();
}

template<>
World<Side::kClient>::~World()
{
  auto client = GetClient();
  client->~Client();
}

template<>
World<Side::kServer>::~World()
{
  auto server = GetServer();
  server->~Server();
}

template<>
NetworkState
World<Side::kClient>::GetNetworkState() const
{
  auto client = GetClient();
  return client->GetNetworkState();
}

template<>
NetworkState
World<Side::kServer>::GetNetworkState() const
{
  auto server = GetServer();
  return server->GetNetworkState();
}
}
