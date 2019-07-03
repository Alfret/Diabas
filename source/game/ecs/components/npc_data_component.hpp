#ifndef NPC_DATA_COMPONENT_HPP_
#define NPC_DATA_COMPONENT_HPP_

#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>
#include "core/types.hpp"
#include "core/uuid.hpp"
#include "core/value_store.hpp"

namespace dib {

struct NpcData
{
  Uuid uuid;
  u32 npc_id;
  ValueStore dynamic_state{};

  bool operator==(const NpcData& other) const { return uuid == other.uuid; }

  bool operator!=(const NpcData& other) const { return !(operator==(other)); }

  bool ToBytes(alflib::RawMemoryWriter& mw) const
  {
    mw.Write(uuid);
    mw.Write(npc_id);
    return mw.Write(dynamic_state);
  }

  static NpcData FromBytes(alflib::RawMemoryReader& mr)
  {
    NpcData data{};
    data.uuid = mr.Read<decltype(uuid)>();
    data.npc_id = mr.Read<decltype(npc_id)>();
    data.dynamic_state = mr.Read<decltype(dynamic_state)>();
    return data;
  }
};

}

#endif // NPC_DATA_COMPONENT_HPP_
