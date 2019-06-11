#include "game.hpp"
#include "app/app.hpp"
#include <dlog.hpp>
#include "network/network.hpp"

#include "network/server.hpp"
#include <thread>
#include <chrono>

int
main(int, char**)
{
  DLOG_INIT();
  DLOG_SET_LEVEL(dlog::Level::kVerbose);
  DLOG_INFO("¸,ø¤º°`°º¤ø,¸  D I A B A S ¸,ø¤º°`°º¤ø,¸");
  DLOG_INFO("running as server");

  if (!dib::Network::InitNetwork()) {
    DLOG_ERROR("Failed to init network.");
    return 1;
  }

  dib::Server server{};
  server.StartServer(24812);
  for (;;) {
    server.Poll();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

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
