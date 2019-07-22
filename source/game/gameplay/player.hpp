#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "core/types.hpp"

namespace dib::game {

class GameClient;

class Player
{
public:
  /**
   * Take input from the user and apply them both locally, and send them
   * to the server.
   */
  static void Update(GameClient& game, const f64 delta);

private:
};
}

#endif // PLAYER_HPP_
