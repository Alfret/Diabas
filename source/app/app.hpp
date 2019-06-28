#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/common.hpp"
#include "core/types.hpp"
#include "network/side.hpp"

// ========================================================================== //
// Application Declaration
// ========================================================================== //

namespace dib::app {

/** Base application class **/
class App
{
public:
  /** Destroy application **/
  virtual ~App() = default;

  /** Run the application loop **/
  virtual void Run() = 0;

  /** Exit application loop **/
  virtual void Exit() = 0;

  /** Called to update application **/
  virtual void Update(f64 delta) = 0;
};
}