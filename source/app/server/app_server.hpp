#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "app/app.hpp"

// ========================================================================== //
// AppServer Declaration
// ========================================================================== //

namespace dib::app {

/** Server application **/
class AppServer : public App
{
public:
  /** Server application descriptor **/
  struct Descriptor
  {
    /** Target 'updates per second' **/
    u32 targetUps = 60;
  };

private:
  /** Target UPS **/
  u32 mTargetUps;

  /** Whether application is running **/
  bool mRunning = false;

public:
  /** Construct server application **/
  explicit AppServer(const Descriptor& descriptor);

  /** Destruct **/
  ~AppServer() override;

  /** Run server application **/
  void Run() override;

  /** Exit application **/
  void Exit() override;
};

}
