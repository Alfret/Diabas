#ifndef NPC_SLIME_HPP_
#define NPC_SLIME_HPP_

#include "game/npc/npc.hpp"

namespace dib::game
{
class NpcSlime : public Npc
{
 public:
   NpcSlime(EntityManager& em, NpcID id, Moveable m, Soul s, RenderComponent rc)
       : Npc(em, id, m, s, rc)
   {}

   void Update(World& world, f64 delta) final;

   void OnSpawn(World& world) final;

   void OnDeath(World& world) final;

   bool Store(const EntityManager& em, alflib::RawMemoryWriter& mw) const final;

   void Load(EntityManager& em, alflib::RawMemoryReader& mr) final;

 private:

};

Npc*
SlimeFactory(EntityManager& em, NpcID id);
}

#endif//NPC_SLIME_HPP_
