#ifndef NPC_RABBIT_HPP_
#define NPC_RABBIT_HPP_

#include "game/npc/npc.hpp"
#include "game/npc/brain/brain_simple.hpp"

namespace dib::game {
class NpcRabbit : public Npc
{
public:
  NpcRabbit(EntityManager& em,
            NpcID id,
            NpcType type,
            Moveable m,
            Soul s,
            RenderComponent rc)
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
RabbitFactory(EntityManager& em, NpcID id, NpcType type);
}

#endif // NPC_RABBIT_HPP_
