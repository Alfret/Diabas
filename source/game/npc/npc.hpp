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

namespace dib::game {

class World;
class NpcRegistry;

/**
 * How make a new custom npc:
 *   1. Derive a class from the Npc class.
 *   2. Make a factory function, that default constructs your own Npc
 *   3. Register the new Npc in npc_registry's constructor.
 */
class Npc
{
public:
  Npc(EntityManager& em,
      NpcID id,
      NpcType type,
      Moveable m,
      Soul s,
      RenderComponent rc);

  virtual ~Npc() = default;

  /// Each frame, update will be called. Typically you do things like
  /// deciding how to move the characters, by setting the moveable.input.
  virtual void Update(World& world, f64 delta);

  // TODO
  virtual void OnSpawn(World& world);

  // TODO
  virtual void OnDeath(World& world);

  auto GetID() const { return id_; }

  auto GetEntity() const { return entity_; }

  auto GetType() const { return type_; }

  /// Store all data via the memory writer.
  /// NOTE: Must call this base method from derived methods.
  virtual bool Store(const EntityManager& em,
                     alflib::RawMemoryWriter& mw) const;

  /// Load the data from the memory reader.
  /// NOTE: must call this base method from derived methods.
  virtual void Load(EntityManager& em, alflib::RawMemoryReader& mr);

  /// Write the subset of information that may change, frame to frame, to
  /// memory writer.
  virtual bool ToIncrement(const EntityManager& em,
                           alflib::RawMemoryWriter& mw) const;
  /// Read the subset of information that may change, frame to frame, from
  /// memory reader.
  virtual void FromIncrement(EntityManager& em, alflib::RawMemoryReader& mr);

protected:
  NpcID id_;
  NpcType type_;
  Entity entity_;
};

void
UpdateNpcs(World& world, f64 delta);

}

#endif // NPC_HPP_
