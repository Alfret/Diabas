#ifndef UUID_COMPONENT_HPP_
#define UUID_COMPONENT_HPP_

#include <uuid.h>
#include "core/types.hpp"
#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>

namespace dib {

/**
 * Generate a uuid value with uuids::uuid_system_generator{}();
 */
struct Uuid
{
  uuids::uuid uuid;

  bool operator==(const Uuid& other) const { return uuid == other.uuid; }

  bool operator!=(const Uuid& other) const { return !(operator==(other)); }

  String ToString() const { return uuids::to_string(uuid); }

  bool ToBytes(alflib::RawMemoryWriter& mw) const
  {
    u8 buf[sizeof(uuid)];
    std::memcpy(buf, &uuid, sizeof(uuid));
    return mw.WriteBytes(buf, sizeof(uuid));
  }

  static Uuid FromBytes(alflib::RawMemoryReader& mr)
  {
    Uuid uuid{};
    const u8* buf = mr.ReadBytes(sizeof(uuid));
    std::memcpy(&uuid, buf, sizeof(uuid));
    return uuid;
  }
};
}

#endif // UUID_COMPONENT_HPP_
