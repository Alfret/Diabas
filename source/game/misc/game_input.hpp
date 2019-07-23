#ifndef GAME_INPUT_HPP_
#define GAME_INPUT_HPP_

#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>
#include <bitset>
#include "core/types.hpp"

namespace dib::game {

class GameInput
{
public:
  bool Left() const { return b_[kLeft]; }
  bool Right() const { return b_[kRight]; }
  bool Jump() const { return b_[kJump]; }

  void ActionLeft() { b_.set(kLeft); }
  void ActionRight() { b_.set(kRight); }
  void ActionJump() { b_.set(kJump); }

private:
  static constexpr u32 kLeft = 0;
  static constexpr u32 kRight = 1;
  static constexpr u32 kJump = 2;
  std::bitset<3> b_;

public:
  bool ToBytes(alflib::RawMemoryWriter& mw) const
  {
    return mw.Write(b_.to_ulong());
  }

  static GameInput FromBytes(alflib::RawMemoryReader& mr)
  {
    GameInput p{};
    p.b_ = mr.Read<unsigned long>();
    return p;
  }
};

}

#endif // GAME_INPUT_HPP_
