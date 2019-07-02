#include "player_system.hpp"
#include "network/connection_state.hpp"

namespace dib::system {

std::optional<const PlayerData*>
PlayerDataFromConnectionId(entt::registry& registry,
                           const ConnectionId connection_id)
{
  auto view = registry.view<PlayerData>();
  for (u32 entity : view) {
    if (connection_id == view.get(entity).connection_id) {
      return &view.get(entity);
    }
  }
  return std::nullopt;
}

}
