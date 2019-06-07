#pragma once

#include "app/app.hpp"

// ========================================================================== //
// Game Declaration
// ========================================================================== //

namespace dib
{

/** Game class**/
class Game : public Application
{
public:
  explicit Game(const Descriptor& descriptor);

  ~Game() override;

  void Update(f64 delta) override;

  void Render() override;

};

}