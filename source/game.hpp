#pragma once

#include "app/app.hpp"
#include "script/env.hpp"

// ========================================================================== //
// Game Declaration
// ========================================================================== //

namespace dib {

/** Game class**/
class Game : public Application
{
private:
  /** Script environment **/
  script::Environment mScriptEnvironment;

public:
  explicit Game(const Descriptor& descriptor);

  ~Game() override;

  void Update(f64 delta) override;

  void Render() override;
};

}