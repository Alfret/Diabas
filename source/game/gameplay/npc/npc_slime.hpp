#ifndef NPC_SLIME_HPP_
#define NPC_SLIME_HPP_

#include "game/npc/npc.hpp"
#include "game/npc/brain/brain_simple.hpp"

namespace dib::game
{
class NpcSlime : public Npc
{
 public:
  NpcSlime(EntityManager& em, NpcID id, NpcType type, Moveable m, Soul s, RenderComponent rc)
      : Npc(em, id, type, m, s, rc)
   {}

   void Update(World& world, f64 delta) final;

   void OnSpawn(World& world) final;

   void OnDeath(World& world) final;

   bool Store(const EntityManager& em, alflib::RawMemoryWriter& mw) const final;

   void Load(EntityManager& em, alflib::RawMemoryReader& mr) final;

 private:
  BrainSimple brain{};
};

Npc*
SlimeFactory(EntityManager& em, NpcID id, NpcType type);
}

#endif//NPC_SLIME_HPP_
