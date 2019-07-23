#ifndef SOUL_HPP_
#define SOUL_HPP_

#include "core/types.hpp"
#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>

namespace dib::game {

class Soul
{
public:
  s32 hp = 100;
  s32 defense = 1;
  f32 damage_timeout = -1.0f;

  // ============================================================ //

  /**
   * @return If we could apply the damage to the soul.
   */
  bool ApplyDamage(s32 damage);

  void DamageTimeoutUpdate(const f32 delta);

  // ============================================================ //

public:
  bool ToBytes(alflib::RawMemoryWriter& mw) const
  {
    mw.Write(hp);
    mw.Write(defense);
    return mw.Write(damage_timeout);
  }

  static Soul FromBytes(alflib::RawMemoryReader& mr)
  {
    Soul data{};
    data.hp = mr.Read<decltype(hp)>();
    data.defense = mr.Read<decltype(defense)>();
    data.damage_timeout = mr.Read<decltype(damage_timeout)>();
    return data;
  }
};

class World;

void
UpdateSoul(World& world, f64 delta);

}

#endif // SOUL_HPP_
