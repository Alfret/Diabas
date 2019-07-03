#ifndef UUID_HPP_
#define UUID_HPP_

#include "core/types.hpp"
#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>
#include <uuid.h>

namespace dib {

struct Uuid
{
  uuids::uuid uuid;

  void GenerateUuid() { uuid = uuids::uuid_system_generator{}(); }

  bool operator==(const Uuid& other) const { return uuid == other.uuid; }

  bool operator!=(const Uuid& other) const { return !(operator==(other)); }

  String ToString() const { return uuids::to_string(uuid); }

  inline friend std::ostream& operator<<(std::ostream& os, const Uuid& uuid)
  {
    return os << uuid.ToString();
  }

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

#endif // UUID_HPP_
