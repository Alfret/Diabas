#ifndef ITEM_DATA_COMPONENT_HPP_
#define ITEM_DATA_COMPONENT_HPP_

#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>
#include "core/types.hpp"
#include "core/uuid.hpp"

namespace dib {

struct ItemData
{
  Uuid uuid;
  // ItemStack item_stack;

  bool operator==(const ItemData& other) const { return uuid == other.uuid; }

  bool operator!=(const ItemData& other) const { return !(operator==(other)); }

  bool ToBytes(alflib::RawMemoryWriter& mw) const
  {
    return mw.Write(uuid);
    // return mw.Write(item_stack);
  }

  static ItemData FromBytes(alflib::RawMemoryReader& mr)
  {
    ItemData data{};
    data.uuid = mr.Read<decltype(uuid)>();
    // data.ItemStack = mr.Read<decltype(item_stack)>();
    return data;
  }
};

}

#endif // ITEM_DATA_COMPONENT_HPP_
