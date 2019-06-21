// ========================================================================== //
// Main
// ========================================================================== //

#include <dlog.hpp>
#include "app/app.hpp"
#include "game/game.hpp"
#include "network/side.hpp"
#include "core/types.hpp"

int
main(int, char**)
{
  DLOG_INIT();
  DLOG_SET_LEVEL(dlog::Level::kVerbose);
  DLOG_INFO("¸,ø¤º°`°º¤ø,¸  D I A B A S  ¸,ø¤º°`°º¤ø,¸");

  // Create and run game
  dib::Application::Descriptor appDescriptor{};
  if (dib::kSide == dib::Side::kServer) {
    appDescriptor.title = dib::String("Diabas - Server");
  } else {
    appDescriptor.title = dib::String("Diabas - Client");
  }
  appDescriptor.width = 1280;
  appDescriptor.height = 720;
  dib::Game app(appDescriptor);
  app.Run();

  return 0;
}
