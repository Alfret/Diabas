#ifndef PLAYER_SYSTEM_HPP_
#define PLAYER_SYSTEM_HPP_

#include <entt/entt.hpp>
#include "game/ecs/components/player_data_component.hpp"

namespace dib::system {

/**
 * Will create an entity with the given PlayerData. If the uuid in player_data
 * is not unique, the creation will fail.
 *
 * @return If we created the new entry.
 */
bool
PlayerDataCreate(entt::registry& registry, const PlayerData& player_data);

/**
 * Will replace the PlayerData for the entity with player_data.uuid's uuid.
 */
bool
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
