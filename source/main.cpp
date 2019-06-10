#include "game.hpp"

// ========================================================================== //
// Main
// ========================================================================== //

#include "app/app.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include <dlog.hpp>
#include "mods/loader.hpp"

int
main(int, char**)
{
  DLOG_INIT();
  DLOG_SET_LEVEL(dlog::Level::kVerbose);
  DLOG_INFO("¸,ø¤º°`°º¤ø,¸  D I A B A S  ¸,ø¤º°`°º¤ø,¸");

  SteamDatagramErrMsg errMsg;
  if (!GameNetworkingSockets_Init(nullptr, errMsg))
    DLOG_ERROR("GameNetworkingSockets_Init failed.  {}", errMsg);

  dib::ModLoader::Test();

  // Create and run game
  dib::Application::Descriptor appDescriptor{};
  appDescriptor.title = "Diabas";
  appDescriptor.width = 1280;
  appDescriptor.height = 720;
  dib::Game app(appDescriptor);
  app.Run();

  return 0;
}
