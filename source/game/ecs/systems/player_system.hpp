#ifndef PLAYER_SYSTEM_HPP_
#define PLAYER_SYSTEM_HPP_

#include <entt/entt.hpp>
#include "game/ecs/components/player_data_component.hpp"

namespace dib::system {

/**
 *
 */
std::optional<const PlayerData*>
PlayerDataFromConnectionId(entt::registry& registry, const ConnectionId connection_id);

}

#endif // PLAYER_SYSTEM_HPP_
