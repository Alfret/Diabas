#ifndef PLAYER_DATA_COMPONENT_HPP_
#define PLAYER_DATA_COMPONENT_HPP_

#include "core/types.hpp"
#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>

namespace dib {
struct PlayerData
{
  String name;
  u64 xp;
  u8 player_class;
  u8 body;
  u8 direction;
  float speed;

  bool ToBytes(alflib::RawMemoryWriter& mw) const
  {
    mw.Write(name);
    mw.Write(xp);
    mw.Write(player_class);
    mw.Write(body);
    mw.Write(direction);
    return mw.Write(speed);
  }

  static PlayerData FromBytes(alflib::RawMemoryReader& mr)
  {
    PlayerData data{};
    data.name = mr.Read<decltype(name)>();
    data.xp = mr.Read<decltype(xp)>();
    data.player_class = mr.Read<decltype(player_class)>();
    data.body = mr.Read<decltype(body)>();
    data.direction = mr.Read<decltype(direction)>();
    data.speed = mr.Read<decltype(speed)>();
    return data;
  }

  String ToString() const
  {
    String str{};
    str += "name: " + name + ", ";
    str += "xp: " + String::ToString(xp) + ", ";
    str += "player_class: " + String::ToString(player_class) + ", ";
    str += "body: " + String::ToString(body) + ", ";
    str += "direction: " + String::ToString(direction) + ", ";
    str += "speed: " + String::ToString(speed);
    return str;
  }
};
}

#endif // PLAYER_DATA_COMPONENT_HPP_
