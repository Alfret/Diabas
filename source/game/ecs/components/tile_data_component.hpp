#ifndef TILE_DATA_COMPONENT_HPP_
#define TILE_DATA_COMPONENT_HPP_

#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>
#include "core/types.hpp"
#include "core/uuid.hpp"
#include "core/value_store.hpp"

namespace dib {

struct TileData
{
  Uuid uuid;
  u32 tile_id;
  ValueStore dynamic_state{};

  bool operator==(const TileData& other) const { return uuid == other.uuid; }

  bool operator!=(const TileData& other) const { return !(operator==(other)); }

  bool ToBytes(alflib::RawMemoryWriter& mw) const
  {
    mw.Write(uuid);
    mw.Write(tile_id);
    return mw.Write(dynamic_state);
  }

  static TileData FromBytes(alflib::RawMemoryReader& mr)
  {
    TileData data{};
    data.uuid = mr.Read<decltype(uuid)>();
    data.tile_id = mr.Read<decltype(tile_id)>();
    data.dynamic_state = mr.Read<decltype(dynamic_state)>();
    return data;
  }
};
}

#endif // TILE_DATA_COMPONENT_HPP_
