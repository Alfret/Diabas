#ifndef NPC_HPP_
#define NPC_HPP_

#include "core/types.hpp"
#include "game/gameplay/moveable.hpp"
#include "game/gameplay/soul.hpp"
#include "game/npc/npc_id.hpp"
#include "game/client/render_component.hpp"
#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>
#include "game/ecs/entity_manager.hpp"

namespace dib::game
{

class World;
class NpcRegistry;

class Npc
{
 private:
  /// construct but defer creation of render component
  //Npc(EntityManager& em, NpcID id, Moveable m, Soul s);

 public:

  Npc(EntityManager& em, NpcID id, Moveable m, Soul s, RenderComponent rc);

  virtual ~Npc() = default;

  /// Each frame, update will be called
  virtual void Update(World& world, f64 delta);

  virtual void OnSpawn(World& world);

  virtual void OnDeath(World& world);

  auto GetEntity() const { return entity_; }

  /// Store all data via the memory writer.
  /// NOTE: Must call this base method from derived methods.
  virtual bool Store(const EntityManager& em, alflib::RawMemoryWriter& mw) const;

  /// Load the data from the memory reader.
  /// NOTE: must call this base method from derived methods.
  virtual void Load(EntityManager& em, alflib::RawMemoryReader& mr);

protected:
  NpcID id_;
  Entity entity_;
};
}

#endif//NPC_HPP_
