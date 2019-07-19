#include "npc_registry.hpp"
#include <dlog.hpp>

/// Npc factory functions
#include "game/gameplay/npc/npc_slime.hpp"

namespace dib::game {

NpcRegistry::NpcRegistry()
{
  RegisterNpcType("slime", SlimeFactory);
}

NpcRegistry::~NpcRegistry() {}

void
NpcRegistry::Add(NpcID id, Npc* npc)
{
  AlfAssert(npcs_.find(id) == npcs_.end(),
            "NpcID is not unique");
  npcs_.insert({id, npc});
}

void
NpcRegistry::Add(EntityManager& em, NpcID id, NpcType type)
{
  if (auto f_it = npc_factories_.find(type); f_it != npc_factories_.end()) {
    Npc* npc = f_it->second(em, id);
    if (npcs_.find(id) == npcs_.end()) {
      npcs_.insert({ id, npc });
    } else {
      DLOG_ERROR("npc id not unique when adding a new npc");
    }
  } else {
    DLOG_ERROR("failed to add npc of type [{}] with id [{}]", type, id);
  }
}

void
NpcRegistry::Remove(const NpcID id)
{
  if (auto it = npcs_.find(id); it != npcs_.end()) {
    delete it.value();
    npcs_.erase(it);
  }
}

Npc*
NpcRegistry::Get(const NpcID id)
{
  if (auto it = npcs_.find(id); it != npcs_.end()) {
    return it->second;
  }
  return nullptr;
}

void
NpcRegistry::RegisterNpcType(const String& type_name, NpcFactoryFunction fn)
{
  static NpcType type_num = 0;
  npc_type_names_.insert({ type_num, type_name });
  npc_factories_.insert({ type_num, fn });
  ++type_num;
}

}
