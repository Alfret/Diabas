#include "game.hpp"

// ========================================================================== //
// Main
// ========================================================================== //

#include "app/app.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include <dlog.hpp>

// TEMP NET HEADERS
#include "network/network.hpp"
#include "network/client.hpp"
#include <chrono>
#include <thread>

int
main(int, char**)
{
  DLOG_INIT();
  DLOG_SET_LEVEL(dlog::Level::kVerbose);
  DLOG_INFO("¸,ø¤º°`°º¤ø,¸  D I A B A S  ¸,ø¤º°`°º¤ø,¸");

  if (!dib::Network::InitNetwork()) {
    DLOG_ERROR("Failed to init network.");
    return 1;
  }

  // ============================================================ //
  // TEMP NET CODE
  // ============================================================ //

  {
    dib::Client client{};
    SteamNetworkingIPAddr addr{};
    addr.SetIPv4(0x7F000001, 24812);
    client.Connect(addr);

    dib::Packet packet{};
    for (;;) {
      client.Poll(packet);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  // ============================================================ //

  // Create and run game
  dib::Application::Descriptor appDescriptor{};
  appDescriptor.title = "Diabas";
  appDescriptor.width = 1280;
  appDescriptor.height = 720;
  dib::Game app(appDescriptor);
  app.Run();

  dib::Network::ShutdownNetwork();

  return 0;
}
