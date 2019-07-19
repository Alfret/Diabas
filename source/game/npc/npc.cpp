#include "npc.hpp"
#include "game/world.hpp"

namespace dib::game
{

Npc::Npc(EntityManager& em, NpcID id, NpcType type, Moveable m, Soul s, RenderComponent rc)
    : id_(id), type_(type)
{
  auto& registry = em.GetRegistry();
  entity_ = registry.create();
  registry.assign<Moveable>(entity_, m);
  registry.assign<Soul>(entity_, s);
  registry.assign<RenderComponent>(entity_, rc);
}

void
Npc::Update(World&, f64)
{}

void
Npc::OnSpawn(World&)
{}

void
Npc::OnDeath(World&)
{}

bool
Npc::Store(const EntityManager& em, alflib::RawMemoryWriter& mw) const
{
  mw.Write(id_);
  mw.Write(type_);
  mw.Write(em.GetRegistry().get<Moveable>(entity_));
  return mw.Write(em.GetRegistry().get<Soul>(entity_));
}

void
Npc::Load(EntityManager& em, alflib::RawMemoryReader& mr)
{
  id_ = mr.Read<decltype(id_)>();
  type_ = mr.Read<decltype(type_)>();
  em.GetRegistry().assign_or_replace<Moveable>(entity_) = mr.Read<Moveable>();
  em.GetRegistry().assign_or_replace<Soul>(entity_) = mr.Read<Soul>();
}
}
