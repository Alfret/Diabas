#ifndef PLAYER_CONNECTION_COMPONENT_HPP_
#define PLAYER_CONNECTION_COMPONENT_HPP_

#include "core/types.hpp"
#include "network/connection_id.hpp"

namespace dib {

struct PlayerConnection
{
  u32 entity_id;
  ConnectionId connection_id;
};

}

#endif // PLAYER_CONNECTION_COMPONENT_HPP_
