#ifndef NPC_SPAWN_HPP_
#define NPC_SPAWN_HPP_

#include "game/npc/npc_id.hpp"
#include "game/npc/npc.hpp"
#include "game/npc/npc_registry.hpp"
#include "network/side.hpp"

namespace dib::game
{

class World;

template <Side side>
void NpcSpawn(World& world, NpcType type, Position position);

}

#endif//NPC_SPAWN_HPP_
