#include "npc_registry.hpp"
#include <dlog.hpp>

/// Npc factory functions
#include "game/gameplay/npc/npc_slime.hpp"
#include "game/gameplay/npc/npc_rabbit.hpp"
#include "game/gameplay/npc/npc_blue_blob.hpp"

namespace dib::game {

NpcRegistry::NpcRegistry()
{
  RegisterNpcType("slime", SlimeFactory);
  RegisterNpcType("rabbit", RabbitFactory);
  RegisterNpcType("blue blob", BlueBlobFactory);
}

NpcRegistry::~NpcRegistry() {}

void
NpcRegistry::Add(EntityManager& em, NpcID id, NpcType type)
{
  if (auto f_it = npc_factories_.find(type); f_it != npc_factories_.end()) {
    Npc* npc = f_it->second(em, id, type);
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
NpcRegistry::Add(EntityManager& em, NpcID id, String type_name)
{
  AlfAssert(npc_type_names_.find(type_name) != npc_type_names_.end(),
            "failed to find npc type [{}]",
            type_name);
  Add(em, id, npc_type_names_[type_name]);
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
  npc_type_names_.insert({ type_name, type_counter_ });
  npc_factories_.insert({ type_counter_, fn });
  ++type_counter_;
}

void
NpcRegistry::Clear()
{
  npcs_.clear();
}

}
