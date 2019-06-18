#include "main.test.hpp"
#include "network/packet_handler.hpp"
#include <dlog.hpp>

using namespace dib;

TEST_SUITE("packet_handler")
{
  TEST_CASE("basic")
  {
    PacketHandler packet_handler{};

    alflib::String names[3] = { "foo", "bar", "ayy" };
    u8 correct_value = 25; // ((10 - 2) * 3) + 1
    u8 value = 10;

    auto FooCb = [&value](const Packet&) { value -= 2; };
    packet_handler.AddPacketType(names[0], FooCb);

    auto BarCb = [&value](const Packet&) { value *= 3; };
    packet_handler.AddPacketType(names[1], BarCb);

    auto AyyCb = [&value](const Packet&) { value += 1; };
    packet_handler.AddPacketType(names[2], AyyCb);

    Packet packet{};
    for (std::size_t i = 0; i < packet_handler.GetSize(); i++) {
      packet_handler[i].callback(packet);
    }
    CHECK(std::memcmp(&value, &correct_value, 1) == 0);
  }

  TEST_CASE("sort")
  {
    PacketHandler packet_handler{};

    alflib::String names[3] = {"foo", "bar", "ayy"};
    u8 correct_value =  25; // ((10 - 2) * 3) + 1
    u8 value = 10;

    auto FooCb = [&value](const Packet&) { value *= 3; };
    packet_handler.AddPacketType(names[0], FooCb);

    auto BarCb = [&value](const Packet&) { value += 1; };
    packet_handler.AddPacketType(names[1], BarCb);

    auto AyyCb = [&value](const Packet&) { value -= 2; };
    packet_handler.AddPacketType(names[2], AyyCb);

    Packet packet{};
    for (std::size_t i=0; i<packet_handler.GetSize(); i++) {
      packet_handler[i].callback(packet);
    }
    CHECK(std::memcmp(&value, &correct_value, 1) != 0);

    value = 10;

    std::vector<PacketMetaSerializable> packet_metas{};
    packet_metas.push_back({ 0, PacketHandler::GetHash(names[2]), names[2] });
    packet_metas.push_back({ 1, PacketHandler::GetHash(names[0]), names[0] });
    packet_metas.push_back({ 2, PacketHandler::GetHash(names[1]), names[1] });

    packet_handler.Sort(packet_metas);

    for (std::size_t i = 0; i < packet_handler.GetSize(); i++) {
      packet_handler[i].callback(packet);
    }
    CHECK(std::memcmp(&value, &correct_value, 1) == 0);
  }
}
