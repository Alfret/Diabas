#ifndef PLAYER_CREATE_SYSTEM_HPP_
#define PLAYER_CREATE_SYSTEM_HPP_

#include <entt/entt.hpp>
#include "game/ecs/components/player_connection_component.hpp"
#include "game/ecs/components/player_data_component.hpp"
#include "game/ecs/components/uuid_component.hpp"
#include "network/connection_id.hpp"
#include "network/connection_state.hpp"
#include <dlog.hpp>
#include "network/side.hpp"

namespace dib::player_create_system {

/**
 * @return Did we update successfully?
 */
inline bool
UpdatePlayerData(entt::registry& registry,
                 Uuid uuid,
                 const PlayerData& player_data)
{
  bool found = false;
  auto view = registry.view<Uuid>();

  for (const auto entity : view) {
    if (uuid == view.get(entity)) {
      DLOG_VERBOSE("updated player data for {}", uuid.ToString());
      registry.assign<PlayerData>(entity, player_data);
      found = true;
      break;
    }
  }

  if (!found) {
    DLOG_WARNING("failed to update player data, uuid {} not found",
                 uuid.ToString());
  }

  return found;
}

/**
 * @return Did we update successfully?
 */
inline bool
UpdateConnection(entt::registry& registry,
                 ConnectionId connection_id,
                 ConnectionState connection_state)
{
  bool found = false;
  bool update_ok = true;
  auto view = registry.view<PlayerConnection>();

  for (auto entity : view) {
    if (view.get(entity).connection_id == connection_id) {
      found = true;
      if (connection_state == ConnectionState::kDisconnected) {
        DLOG_INFO("TODO save the player data before destroy");
        registry.destroy(entity);
      } else {
        update_ok = false;
        DLOG_WARNING("got a connection status update with kConnected, but we"
                     " are already connected.");
      }
      break;
    }
  }

  if (!found) {
    if (connection_state == ConnectionState::kConnected) {
      auto entity = registry.create();
      DLOG_VERBOSE("added connection {} at entity {}", connection_id, entity);
      registry.assign<PlayerConnection>(entity, entity, connection_id);
    } else {
      update_ok = false;
      DLOG_WARNING("connection was not found, so the connection_state should"
                   " be kConnected, but was kDisconnected");
    }
  }

  return update_ok;
}

/**
 * @return Did we update successfully?
 */
inline bool
UpdateUuid(entt::registry& registry, ConnectionId connection_id, Uuid uuid)
{
  bool found = false;
  auto view = registry.view<PlayerConnection>();

  for (auto entity : view) {
    if (view.get(entity).connection_id == connection_id) {
      found = true;
      registry.assign<Uuid>(entity, uuid);
      break;
    }
  }

  if (!found) {
    DLOG_WARNING("unable to find connection_id when updating uuid");
  }

  return found;
}

}

#endif // PLAYER_CREATE_SYSTEM_HPP_
