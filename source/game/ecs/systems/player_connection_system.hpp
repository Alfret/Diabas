#ifndef PLAYER_CONNECTION_SYSTEM_HPP_
#define PLAYER_CONNECTION_SYSTEM_HPP_

#include <entt/entt.hpp>
#include "game/ecs/components/player_connection_component.hpp"
#include "game/ecs/components/player_data_component.hpp"
#include "network/connection_id.hpp"
#include "network/connection_state.hpp"
#include <dlog.hpp>

namespace dib::player_connection_system {

inline void
Update(entt::registry& registry,
       ConnectionId connection_id,
       const PlayerData& player_data)
{
  bool found = false;
  auto view = registry.view<PlayerConnection>();

  for (const auto entity : view) {
    if (connection_id == view.get(entity).connection_id) {
      DLOG_VERBOSE("updated player data for {}", connection_id);
      registry.assign<PlayerData>(entity, player_data);
      found = true;
      break;
    }
  }

  if (!found) {
    DLOG_WARNING("failed to update player data, connection_id not found");
  }
}

inline void
Update(entt::registry& registry,
       ConnectionId connection_id,
       ConnectionState connection_state)
{
  bool found = false;
  auto view = registry.view<PlayerConnection>();

  for (auto entity : view) {
    if (view.get(entity).connection_id == connection_id) {
      found = true;
      if (connection_state == ConnectionState::kDisconnected) {
        DLOG_INFO("TODO save the player data before destroy");
        registry.destroy(entity);
      } else {
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
      DLOG_WARNING("connection was not found, so the connection_state should"
                   " be kConnected, but was kDisconnected");
    }
  }
}

}

#endif // PLAYER_CONNECTION_SYSTEM_HPP_
