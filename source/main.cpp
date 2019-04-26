#include "app/app.hpp"

#include <iostream>

int
main(int, char**)
{
  using namespace dib;

  Application::Descriptor appDescriptor{};
  appDescriptor.title = "Diabas";
  appDescriptor.width = 1280;
  appDescriptor.height = 720;
  appDescriptor.api = Graphics::API::D3D11;

  Application app(appDescriptor);
  app.GetGraphics().SetClearColor(100/255.0f, 149/255.0f, 237/255.0f, 1.0f);

  const String& deviceName = app.GetGraphics().DeviceName();

  app.Run();


  return 0;
}
