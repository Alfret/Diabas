#include "app/server/app_server.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dutil/stopwatch.hpp>

// ========================================================================== //
// AppServer Implementation
// ========================================================================== //

namespace dib::app {

AppServer::AppServer(const AppServer::Descriptor& descriptor)
  : mTargetUps(descriptor.targetUps)
{}

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
    // Update time variables
    const f64 timeCurrent = sw.fnow_s();
    const f64 timeDelta = timeCurrent - timeLast;
    timeLast = timeCurrent;

    // Update
    Update(timeDelta);
  }
}

// -------------------------------------------------------------------------- //

void
AppServer::Exit()
{
  mRunning = false;
}

}