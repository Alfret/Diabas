// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>
#include "app/app.hpp"
#include "network/side.hpp"
#include "core/types.hpp"

#if defined(DIB_IS_SERVER)
#include "game/server/game_server.hpp"
#else
#include "game/client/game_client.hpp"
#endif

// ========================================================================== //
// Main Function
// ========================================================================== //

int
main(int, char**)
{
  using namespace dib;

  DLOG_INIT();
  DLOG_SET_LEVEL(dlog::Level::kVerbose);
  DLOG_INFO("¸,ø¤º°`°º¤ø,¸  D I A B A S  ¸,ø¤º°`°º¤ø,¸");

  // Run either client or server
#if !defined(DIB_IS_SERVER)
  game::GameClient::Descriptor descriptor;
  descriptor.title = "Diabas - Client";
  descriptor.width = 1280;
  descriptor.height = 720;
  game::GameClient client(descriptor);
  client.Run();
#else
  game::GameServer::Descriptor descriptor;
  game::GameServer server(descriptor);
  server.Run();
#endif

  return 0;
}
