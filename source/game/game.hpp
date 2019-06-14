#pragma once

#include "app/app.hpp"
#include "game/world.hpp"

// ========================================================================== //
// Game Declaration
// ========================================================================== //

namespace dib {

/** Game class**/
class Game : public Application
{
public:
  explicit Game(const Descriptor& descriptor);

  ~Game() override;

  void Update(f64 delta) override;

  void Render() override;

private:
#ifdef DIB_IS_SERVER
#define DIB_SIDE Side::kServer
#else
#define DIB_SIDE Side::kClient
#endif
  World<DIB_SIDE> world;
#undef DIB_SIDE
};

}
