#ifndef NPC_REGISTRY_HPP_
#define NPC_REGISTRY_HPP_

#include <vector>
#include <tsl/robin_map.h>
#include "game/npc/npc.hpp"
#include "game/npc/npc_id.hpp"
#include <alflib/memory/raw_memory_reader.hpp>

namespace dib::game {

/**
 * This factory function should allocate memory for a npc type, and
 * initialize it with default values.
 */
using NpcFactoryFunction = Npc* (*)(EntityManager&, NpcID, NpcType);

/**
 * Keeps track of what npc's currently exists, and what npc's could
 * exist in the future.
 */
class NpcRegistry
{
public:
  NpcRegistry();
  ~NpcRegistry();

  /// Add a npc by default constructing the given npc type.
  /// @type The type to default construct
  /// @return Iterator to the newly created npc.
  void Add(EntityManager& em, NpcID id, NpcType type);
  void Add(EntityManager& em, NpcID id, String type_name);

  /// Remove a npc via its id.
  void Remove(NpcID id);

  /// @return Ptr to npc on success, nullptr on fail.
  Npc* Get(NpcID);

  void RegisterNpcType(const String& type_name, NpcFactoryFunction fn);

  auto& GetNpcs() { return npcs_; }
  const auto& GetNpcs() const { return npcs_; }

  const auto& GetNpcTypeNames() const { return npc_type_names_; }

  /// Clear all data we have.
  void Clear();

private:
  /// Collection of all NPC's.
  /// PERF: it would better to store similar npc together. As of now, they
  /// appear in order of creation.
  tsl::robin_map<NpcID, Npc*> npcs_;

  /// Stores a name with the npc type. This is useful for synchronizing
  /// between server client to ensure consistent types.
  tsl::robin_map<String, NpcType> npc_type_names_;

  /// From a serialized npc type, a factory function for it exists.
  tsl::robin_map<NpcType, NpcFactoryFunction> npc_factories_;

  /// Internal
  NpcType type_counter_ = 0;
};
}

#endif // NPC_REGISTRY_HPP_
