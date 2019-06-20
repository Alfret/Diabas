#include "network.hpp"
#include <chrono>
#include <thread>
#include "core/fixed_time_update.hpp"
#include <functional>

namespace dib {

template<>
void
Network<Side::kClient>::Update()
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
Network<Side::kServer>::Update()
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
Network<Side::kClient>::ConnectToServer()
{
  SteamNetworkingIPAddr addr{};
  addr.SetIPv4(0x7F000001, kPort);
  auto client = GetClient();
  client->Connect(addr);
}

template<>
void
Network<Side::kServer>::StartServer()
{
  auto server = GetServer();
  server->StartServer(kPort);
}

template<>
void
Network<Side::kClient>::SetupPacketHandler()
{
  const auto SyncCb = [](const Packet&) {
    DLOG_INFO("TODO handle sync packets");
  };
  bool ok = packet_handler_.AddStaticPacketType(
    PacketHeaderStaticTypes::kSync, "sync", SyncCb);
  AlfAssert(ok, "could not add packet type sync");

  const auto ChatCb = [](const Packet& packet) {
    alflib::String msg = packet.ToString();
    DLOG_RAW("Server: {}\n", msg);
  };
  ok = packet_handler_.AddStaticPacketType(PacketHeaderStaticTypes::kChat, "chat", ChatCb);
  AlfAssert(ok, "could not add packet type chat");
}

template<>
void
Network<Side::kServer>::SetupPacketHandler()
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
  ok = packet_handler_.AddStaticPacketType(PacketHeaderStaticTypes::kChat, "chat", ChatCb);
  AlfAssert(ok, "could not add packet type chat");
}

template<>
void
Network<Side::kServer>::NetworkInfo([
  [maybe_unused]] const std::string_view message) const
{
  auto server = GetServer();
  server->NetworkInfo();
}

template<>
void
Network<Side::kClient>::NetworkInfo([
  [maybe_unused]] const std::string_view message) const
{}

template<>
void
Network<Side::kServer>::Broadcast(const std::string_view message) const
{
  auto server = GetServer();
  Packet packet{ message.data(), message.size() };
  packet_handler_.BuildPacketHeader(packet, PacketHeaderStaticTypes::kChat);
  server->BroadcastPacket(packet, SendStrategy::kReliable);
}

template<>
void
Network<Side::kClient>::Broadcast([
  [maybe_unused]] const std::string_view message) const
{}

template<>
Network<Side::kClient>::Network()
{
  if (!Network<Side::kClient>::InitNetwork()) {
    DLOG_ERROR("Failed to init network.");
    std::exit(1);
  }
  SetupPacketHandler();
  base_ = new Client(&packet_handler_);
  ConnectToServer();
}

template<>
Network<Side::kServer>::Network()
{
  if (!Network<Side::kServer>::InitNetwork()) {
    DLOG_ERROR("Failed to init network.");
    std::exit(1);
  }
  base_ = new Server(&packet_handler_);
  SetupPacketHandler();
  StartServer();
}

template<>
Network<Side::kClient>::~Network()
{
  auto client = GetClient();
  client->~Client();
  Network<Side::kClient>::ShutdownNetwork();
}

template<>
Network<Side::kServer>::~Network()
{
  auto server = GetServer();
  server->~Server();
  Network<Side::kServer>::ShutdownNetwork();
}

template<>
NetworkState
Network<Side::kClient>::GetNetworkState() const
{
  auto client = GetClient();
  return client->GetNetworkState();
}

template<>
NetworkState
Network<Side::kServer>::GetNetworkState() const
{
  auto server = GetServer();
  return server->GetNetworkState();
}

// ============================================================ //

void
DebugOutputCallback(ESteamNetworkingSocketsDebugOutputType type,
                    const char* msg)
{
  DLOG_VERBOSE("Steam Socket debug output: [{}] [{}]", type, msg);
}

void
SleepAndKill()
{
  // Give the sockets time to finish up, not sure if needed.
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  GameNetworkingSockets_Kill();
}

// ============================================================ //

}
