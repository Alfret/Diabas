#include "app/app.hpp"
#include <dlog/dlog.hpp>
#include <iostream>

int
main(int, char**)
{
  using namespace dib;

  DLOG_INIT();
  DLOG_SET_LEVEL(dlog::Level::kVerbose);
  DLOG_INFO("¸,ø¤º°`°º¤ø,¸  D I A B A S  ¸,ø¤º°`°º¤ø,¸");

  Application::Descriptor appDescriptor{};
  appDescriptor.title = "Diabas";
  appDescriptor.width = 1280;
  appDescriptor.height = 720;

#if defined(DIB_TARGET_WINDOWS)
  appDescriptor.api = Graphics::API::D3D11;
#elif defined(DIB_TARGET_LINUX)
  appDescriptor.api = Graphics::API::OpenGL;
#endif

  Application app(appDescriptor);
  app.GetGraphics().SetClearColor(100/255.0f, 149/255.0f, 237/255.0f, 1.0f);

  const String& deviceName = app.GetGraphics().DeviceName();

  app.Run();


  return 0;
}
