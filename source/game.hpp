#pragma once

#include "app/app.hpp"
#include "core/fixed_time_update.hpp"
#include "network/client.hpp"

// ========================================================================== //
// Game Declaration
// ========================================================================== //



namespace dib
{

void Ayy();

/** Game class**/
class Game : public Application
{
public:
  explicit Game(const Descriptor& descriptor);

  ~Game() override;

  void Update(f64 delta) override;

  void Render() override;

 private:
   void UpdateNetwork();

  void ConnectToServer();

 private:
  static constexpr s64 kNetTicksPerSec = 32;
   Client client_{};
};

}
