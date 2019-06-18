#pragma once

#include "app/app.hpp"
#include "script/environment.hpp"

// ========================================================================== //
// Game Declaration
// ========================================================================== //

namespace dib
{

/** Game class**/
class Game : public Application
{
private:
  /** Script environment **/
  script::ScriptEnvironment mScriptEnvironment;

public:
  explicit Game(const Descriptor& descriptor);

  ~Game() override;

  void Update(f64 delta) override;

  void Render() override;

};

}