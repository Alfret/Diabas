#ifndef NPC_SPAWN_HPP_
#define NPC_SPAWN_HPP_

#include "game/npc/npc_id.hpp"
#include "game/npc/npc.hpp"
#include "game/npc/npc_registry.hpp"
#include "network/side.hpp"

namespace dib::game {

class World;

/// kServer: Spawn the npc.
/// kClient: Send a spawn request.
template<Side side>
void
NpcSpawn(World& world, NpcType type, Position position);

/// Use a lookup table for type_name to call the spawn function above.
template<Side side>
void
NpcSpawn(World& world, String type_name, Position position);

}

#endif // NPC_SPAWN_HPP_
