#include "main.test.hpp"
#include "network/packet_handler.hpp"
#include <dlog.hpp>

using namespace dib;

TEST_SUITE("packet_handler")
{
  TEST_CASE("basic")
  {
    PacketHandler packet_handler{};

    u8 value = 10;

    bool ok = packet_handler.AddPacketType("adder", [&value](const Packet& packet) {
      value += *packet.GetPayload();
    });
    CHECK(ok);
    auto adder_type = packet_handler.GetType("adder");
    CHECK(adder_type);

    ok = packet_handler.AddPacketType(
      "ResetAndMultiply",
      [&value](const Packet& packet) { value = 2 * *packet.GetPayload(); });
    CHECK(ok);
    auto reset_type = packet_handler.GetType("ResetAndMultiply");
    CHECK(reset_type);

    Packet packet{1};
    u8 v = 4;
    packet.SetPayload(&v, 1);
    packet_handler[*adder_type].callback(packet);
    CHECK(value == 10+v);

    packet_handler[*reset_type].callback(packet);
    CHECK(value == 10 * 0 + 2 * v);
  }

  TEST_CASE("type/hash collision")
  {
    PacketHandler packet_handler{};
    u8 value = 10;
    const PacketHeaderType same_type = 0;

    bool ok = packet_handler.AddPacketType(same_type, "adder", [&value](const Packet& packet) {
      value += *packet.GetPayload();
    });
    CHECK(ok);
    auto adder_type = packet_handler.GetType("adder");
    CHECK(adder_type);

    ok = packet_handler.AddPacketType(
        same_type,
      "ResetAndMultiply",
      [&value](const Packet& packet) { value = 2 * *packet.GetPayload(); });
    CHECK(ok);
    auto reset_type = packet_handler.GetType("ResetAndMultiply");
    CHECK(reset_type);

    Packet packet{ 1 };
    u8 v = 4;
    packet.SetPayload(&v, 1);
    packet_handler[*adder_type].callback(packet);
    CHECK(value == 10 + v);

    packet_handler[*reset_type].callback(packet);
    CHECK(value == 10 * 0 + 2 * v);
  }

  TEST_CASE("sync")
  {
    PacketHandler packet_handler{};
    u8 value = 10;

    const PacketHeaderType type_x = 0;
    const PacketHeaderType type_y = 500;

    bool ok = packet_handler.AddPacketType(
      type_y, "x", [&value](const Packet& packet) {
        value += *packet.GetPayload();
      });
    CHECK(ok);

    ok = packet_handler.AddPacketType(
      type_x, "y", [&value](const Packet& packet) {
        value = 2 * *packet.GetPayload();
      });
    CHECK(ok);

    // wrong order
    Packet packet{ 1 };
    u8 v = 4;
    packet.SetPayload(&v, 1);
    packet_handler[type_x].callback(packet);
    CHECK(value != 10 + v);
    packet_handler[type_y].callback(packet);
    CHECK(value != 10 * 0 + 2 * v);

    // build up the correct packet handler
    PacketHandler correct_packet_handler{};
    ok =
      correct_packet_handler.AddPacketType(type_x, "x", [&value](const Packet& packet) {
        value += *packet.GetPayload();
      });
    CHECK(ok);
    ok =
      correct_packet_handler.AddPacketType(type_y, "y", [&value](const Packet& packet) {
        value = 2 * *packet.GetPayload();
      });
    CHECK(ok);

    packet_handler.Sync(correct_packet_handler.Serialize());

    // check it is now correct
    value = 10;
    packet_handler[type_x].callback(packet);
    CHECK(value == 10 + v);
    packet_handler[type_y].callback(packet);
    CHECK(value == 10 * 0 + 2 * v);
  }
}
