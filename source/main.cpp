#include "game.hpp"

// ========================================================================== //
// Main
// ========================================================================== //

int
main(int, char**)
{
  // Create and run game
  dib::Application::Descriptor appDescriptor{};
  appDescriptor.title = "Diabas";
  appDescriptor.width = 1280;
  appDescriptor.height = 720;
  dib::Game app(appDescriptor);
  app.Run();

  return 0;
}
