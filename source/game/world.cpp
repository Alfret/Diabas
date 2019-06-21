#include "world.hpp"
#include "core/fixed_time_update.hpp"
#include <dlog.hpp>
#include <functional>
#include <alflib/core/assert.hpp>

namespace dib {

template<>
void
World<Side::kClient>::Update()
{

  // update network
  static bool got_update;
  auto client = GetClient();
  static const auto PollClient =
    std::bind(&Client::Poll, client, std::ref(got_update), std::ref(packet_));
  got_update = false;
  FixedTimeUpdate(kNetTicksPerSec, PollClient);
  if (got_update) {
    packet_handler_.HandlePacket(packet_);
  }
}

template<>
void
World<Side::kServer>::Update()
{

  // update network
  static bool got_update;
  auto server = GetServer();
  static const auto PollServer =
    std::bind(&Server::Poll, server, std::ref(got_update), std::ref(packet_));
  got_update = false;
  FixedTimeUpdate(kNetTicksPerSec, PollServer);
  if (got_update) {
    packet_handler_.HandlePacket(packet_);
  }
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
void
World<Side::kClient>::SetupPacketHandler()
{
  const auto SyncCb = [](const Packet&) {
    DLOG_RAW("TODO handle sync packet\n");
  };
  bool ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kSync, "sync", SyncCb);
  AlfAssert(ok, "could not add packet type sync");

  const auto ChatCb = [](const Packet& packet) {
    alflib::String msg = packet.ToString();
    DLOG_RAW("Server: {}\n", msg);
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kChat, "chat", ChatCb);
  AlfAssert(ok, "could not add packet type chat");
}

template<>
void
World<Side::kServer>::SetupPacketHandler()
{
  const auto SyncCb = [](const Packet&) {
    DLOG_RAW("TODO handle sync packet\n");
  };
  bool ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kSync, "sync", SyncCb);
  AlfAssert(ok, "could not add packet type sync");

  const auto ChatCb = [](const Packet& packet) {
    alflib::String msg = packet.ToString();
    DLOG_RAW("TODO handle chat packets [{}]\n", msg);
  };
  ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kChat, "chat", ChatCb);
  AlfAssert(ok, "could not add packet type chat");
}

template<>
void
World<Side::kServer>::NetworkInfo([
  [maybe_unused]] const std::string_view message) const
{
  auto server = GetServer();
  server->NetworkInfo();
}

template<>
void
World<Side::kClient>::NetworkInfo([
  [maybe_unused]] const std::string_view message) const
{}

template<>
void
World<Side::kServer>::Broadcast(const std::string_view message) const
{
  auto server = GetServer();
  Packet packet{ message.data(), message.size() };
  packet_handler_.BuildPacketHeader(packet, PacketHeaderStaticTypes::kChat);
  server->BroadcastPacket(packet, SendStrategy::kReliable);
}

template<>
void
World<Side::kClient>::Broadcast([
  [maybe_unused]] const std::string_view message) const
{}

template<>
World<Side::kClient>::World()
  : base_(new Client())
{
  SetupPacketHandler();
  ConnectToServer();
}

template<>
World<Side::kServer>::World()
  : base_(new Server(this))
{
  SetupPacketHandler();
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
