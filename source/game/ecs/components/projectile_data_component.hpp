#ifndef PROJECTILE_DATA_COMPONENT_HPP_
#define PROJECTILE_DATA_COMPONENT_HPP_

#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>
#include "core/types.hpp"
#include "core/uuid.hpp"
#include "core/value_store.hpp"

namespace dib {
struct ProjectileData
{
  Uuid uuid;
  u32 projectile_id;
  ValueStore dynamic_state{};

  bool operator==(const ProjectileData& other) const
  {
    return uuid == other.uuid;
  }

  bool operator!=(const ProjectileData& other) const
  {
    return !(operator==(other));
  }

  bool ToBytes(alflib::RawMemoryWriter& mw) const
  {
    mw.Write(uuid);
    mw.Write(projectile_id);
    return mw.Write(dynamic_state);
  }

  static ProjectileData FromBytes(alflib::RawMemoryReader& mr)
  {
    ProjectileData data{};
    data.uuid = mr.Read<decltype(uuid)>();
    data.projectile_id = mr.Read<decltype(projectile_id)>();
    data.dynamic_state = mr.Read<decltype(dynamic_state)>();
    return data;
  }
};
}

#endif // PROJECTILE_DATA_COMPONENT_HPP_
