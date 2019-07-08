#ifndef PLAYER_DATA_STORAGE_HPP_
#define PLAYER_DATA_STORAGE_HPP_

#include "game/ecs/components/player_data_component.hpp"

namespace dib {
class PlayerDataStorage
{
public:
  static PlayerData Load()
  {
    PlayerData data{};
    data.name = "Rully";
    game::MoveableEntity e{};
    e.friction_modifier = 0.7f;
    e.height = game::kTileInMeters * 3;
    e.width = game::kTileInMeters;
    e.horizontal_acceleration_modifier = 10.0f;
    e.horizontal_max_acceleration = 12.0f;
    e.position.x = game::TileToMeter(800);
    e.position.y = game::TileToMeter(20);
    data.moveable_entity = e;
    return data;
  }

private:
};
}

#endif // PLAYER_DATA_STORAGE_HPP_
