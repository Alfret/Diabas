#ifndef NPC_ID_HPP_
#define NPC_ID_HPP_

#include "core/types.hpp"

namespace dib::game
{

/// Identifier for the type of npc.
using NpcType = u32;

/// Identifier for an instance of a npc.
using NpcID = u32;

/// Generates a unique npc id.
NpcID NpcIDGenerateUnique();

}

#endif//NPC_ID_HPP_
