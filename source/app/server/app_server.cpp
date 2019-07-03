#include "app/server/app_server.hpp"
#include <microprofile/microprofile.h>

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dutil/stopwatch.hpp>

// ========================================================================== //
// AppServer Implementation
// ========================================================================== //

namespace dib::app {

MICROPROFILE_DEFINE(MAIN, "MAIN", "Main", MP_IVORY2);

AppServer::AppServer(const AppServer::Descriptor& descriptor)
  : mTargetUps(descriptor.targetUps)
{
  // Microprofile setup
  MicroProfileOnThreadCreate("Main");
  MicroProfileSetEnableAllGroups(true);
  MicroProfileSetForceMetaCounters(true);
}

AppServer::~AppServer()
{
  MicroProfileShutdown();
}

// -------------------------------------------------------------------------- //

void
AppServer::Run()
{
  // Create and start stopwatch
  dutil::Stopwatch sw;
  sw.Start();

  // Run main loop
  f64 timeLast = sw.fnow_s();
  mRunning = true;
  while (mRunning) {
    MICROPROFILE_SCOPE(MAIN);

    // Update time variables
    const f64 timeCurrent = sw.fnow_s();
    const f64 timeDelta = timeCurrent - timeLast;
    timeLast = timeCurrent;

    // Update
    Update(timeDelta);

    MicroProfileFlip(nullptr);
  }
}

// -------------------------------------------------------------------------- //

void
AppServer::Exit()
{
  mRunning = false;
}

}
