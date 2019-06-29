#ifndef PLAYER_SYSTEM_HPP_
#define PLAYER_SYSTEM_HPP_

#include <entt/entt.hpp>
#include "game/ecs/components/player_data_component.hpp"

namespace dib::system {

/**
 * Will create or replace a PlayerData entity.
 */
void
PlayerDataUpdate(entt::registry& registry,
                 const PlayerData& player_data);

/**
 * Find and destroy the entity with the given uuid. Noop if not found.
 */
void
PlayerDataDelete(entt::registry& registry, const Uuid& uuid);
void
PlayerDataDelete(entt::registry& registry, const Uuid* uuid);

/**
 * Find and destroy the entity with the given uuid. Noop if not found.
 */
void
PlayerDataDelete(entt::registry& registry, const PlayerData& player_data);
void
PlayerDataDelete(entt::registry& registry, const PlayerData* player_data);

/**
 *
 */
std::optional<const PlayerData*>
PlayerDataFromConnectionId(entt::registry& registry, const ConnectionId connection_id);

/**
 *
 */
std::optional<const PlayerData*>
PlayerDataFromUuid(entt::registry& registry, const Uuid& uuid);

}

#endif // PLAYER_SYSTEM_HPP_
