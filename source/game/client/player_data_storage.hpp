#ifndef PLAYER_DATA_STORAGE_HPP_
#define PLAYER_DATA_STORAGE_HPP_

#include "game/ecs/components/player_data_component.hpp"
#include "game/physics/collision.hpp"

namespace dib {
class PlayerDataStorage
{
public:
  static PlayerData Load()
  {
    PlayerData data{};
    data.name = "Rully";
    return data;
  }

private:
};
}

#endif // PLAYER_DATA_STORAGE_HPP_
