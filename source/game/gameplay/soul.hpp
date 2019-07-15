#ifndef SOUL_HPP_
#define SOUL_HPP_

#include "core/types.hpp"
#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>

namespace dib::game
{

class Soul
{
 public:

  Soul() : hp_(100), defense_(1), damage_timeout_(-1.0f) {};

  // ============================================================ //

  s32 GetHP() { return hp_; }

  /**
   * @return If we could apply the damage to the soul.
   */
  bool ApplyDamage(s32 damage);

  s32 GetDefense() { return defense_; }

  // ============================================================ //

 public:
  bool ToBytes(alflib::RawMemoryWriter& mw) const
  {
    mw.Write(hp_);
    mw.Write(defense_);
    mw.Write(damage_timeout_);
  }

  static Soul FromBytes(alflib::RawMemoryReader& mr)
  {
    Soul data{};
    data.hp_ = mr.Read<decltype(hp_)>();
    data.defense_ = mr.Read<decltype(defense_)>();
    data.damage_timeout_ = mr.Read<decltype(damage_timeout_)>();
    return data;
  }

  // ============================================================ //

private:
  s32 hp_;
  s32 defense_;
  float damage_timeout_;
};

}

#endif//SOUL_HPP_
