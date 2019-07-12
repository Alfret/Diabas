#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>
#include "core/types.hpp"
#include <bitset>

namespace dib::game {

class GameClient;

class PlayerInput
{
 public:

  bool Left() const { return  b_[kLeft]; }
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

  bool ToBytes(alflib::RawMemoryWriter& mw) const {
    return mw.Write(b_.to_ulong());
  }

  static PlayerInput FromBytes(alflib::RawMemoryReader& mr) {
    PlayerInput p{};
    p.b_ = mr.Read<unsigned long>();
    return p;
  }
};

class Player
{
public:

  /**
   * Take input from the user and apply them both locally, and send them
   * to the server.
   */
  void Update(GameClient& game, const f64 delta);

private:
};
}

#endif // PLAYER_HPP_
