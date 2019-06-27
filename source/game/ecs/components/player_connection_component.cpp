#include "player_connection_component.hpp"

#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include <entt/entt.hpp>

namespace dib {

static_assert(sizeof(PlayerConnection::entity_id) == sizeof(entt::entity),
              "entity type missmatch");

}
