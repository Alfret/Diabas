#ifndef NPC_BLUE_BLOB_HPP_
#define NPC_BLUE_BLOB_HPP_

#include "game/npc/npc.hpp"
#include "game/npc/brain/brain_simple.hpp"
#include "game/npc/brain/brain_follow.hpp"

namespace dib::game {
class NpcBlueBlob : public Npc
{
public:
  NpcBlueBlob(EntityManager& em,
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
  BrainFollow brain_{ entity_ };
  BrainSimple simple_brain_;
};

Npc*
BlueBlobFactory(EntityManager& em, NpcID id, NpcType type);
}

#endif // NPC_BLUE_BLOB_HPP_
