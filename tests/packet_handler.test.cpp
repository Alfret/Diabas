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

    bool ok = packet_handler.AddDynamicPacketType("adder", [&value](const Packet& packet) {
      value += *packet.GetPayload();
    });
    CHECK(ok);

    ok = packet_handler.AddDynamicPacketType(
      "ResetAndMultiply",
      [&value](const Packet& packet) { value = 2 * *packet.GetPayload(); });
    CHECK(ok);

    Packet packet{1};
    u8 v = 4;
    packet.SetPayload(&v, 1);
    packet_handler.BuildPacketHeader(packet, "adder");
    bool did_handle = packet_handler.HandlePacket(packet);
    CHECK(did_handle);
    CHECK(value == 10+v);

    packet_handler.BuildPacketHeader(packet, "ResetAndMultiply");
    did_handle = packet_handler.HandlePacket(packet);
    CHECK(did_handle);
    CHECK(value == 10 * 0 + 2 * v);
  }

  TEST_CASE("Static sync")
  {
    PacketHandler packet_handler{};
    u8 value = 10;

    bool ok = packet_handler.AddStaticPacketType(
        PacketHeaderStaticTypes::kSync, "chat", [&value](const Packet& packet) {
        value += *packet.GetPayload();
      });
    CHECK(ok);

    ok = packet_handler.AddStaticPacketType(
        PacketHeaderStaticTypes::kChat, "sync", [&value](const Packet& packet) {
        value = 2 * *packet.GetPayload();
      });
    CHECK(ok);

    // build up the correct packet handler
    PacketHandler correct_packet_handler{};
    ok =
        correct_packet_handler.AddStaticPacketType(PacketHeaderStaticTypes::kChat, "chat", [&value](const Packet& packet) {
        value += *packet.GetPayload();
      });
    CHECK(ok);
    ok =
        correct_packet_handler.AddStaticPacketType(PacketHeaderStaticTypes::kSync, "sync", [&value](const Packet& packet) {
        value = 2 * *packet.GetPayload();
      });
    CHECK(ok);

    // prepare the two packets
    Packet chat_packet{ 1 };
    correct_packet_handler.BuildPacketHeader(chat_packet, PacketHeaderStaticTypes::kChat);
    u8 v = 4;
    chat_packet.SetPayload(&v, 1);
    Packet sync_packet{ 1 };
    correct_packet_handler.BuildPacketHeader(sync_packet,
                                               PacketHeaderStaticTypes::kSync);
    sync_packet.SetPayload(&v, 1);

    // wrong order
    bool did_handle = packet_handler.HandlePacket(chat_packet);
    CHECK(did_handle);
    CHECK(value != 10 + v);
    did_handle = packet_handler.HandlePacket(sync_packet);
    CHECK(did_handle);
    CHECK(value != 10 * 0 + 2 * v);

    // sync
    const auto res = packet_handler.Sync(correct_packet_handler.Serialize());
    CHECK(res == PacketHandler::SyncResult::kSuccess);

    // check it is now correct
    value = 10;
    did_handle = packet_handler.HandlePacket(chat_packet);
    CHECK(did_handle);
    CHECK(value == 10 + v);
    did_handle = packet_handler.HandlePacket(sync_packet);
    CHECK(did_handle);
    CHECK(value == 10 * 0 + 2 * v);
  }

  TEST_CASE("Dynamic sync")
  {
    PacketHandler packet_handler{};
    u8 value = 10;

    bool ok = packet_handler.UnsafeAddDynamicPacketType(
        "a", 11, [&value](const Packet& packet) {
        value += *packet.GetPayload();
      });
    CHECK(ok);

    ok = packet_handler.UnsafeAddDynamicPacketType(
        "b", 10, [&value](const Packet& packet) {
        value = 2 * *packet.GetPayload();
      });
    CHECK(ok);

    // build up the correct packet handler
    PacketHandler correct_packet_handler{};
    ok = correct_packet_handler.AddDynamicPacketType(
      "a", [&value](const Packet& packet) { value += *packet.GetPayload(); });
    CHECK(ok);
    ok = correct_packet_handler.AddDynamicPacketType(
      "b",
      [&value](const Packet& packet) { value = 2 * *packet.GetPayload(); });
    CHECK(ok);

    // prepare the two packets
    Packet a_packet{ 1 };
    correct_packet_handler.BuildPacketHeader(a_packet,
                                               "a");
    u8 v = 4;
    a_packet.SetPayload(&v, 1);
    Packet b_packet{ 1 };
    correct_packet_handler.BuildPacketHeader(b_packet,
                                               "b");
    b_packet.SetPayload(&v, 1);

    // wrong order
    bool did_handle = packet_handler.HandlePacket(a_packet);
    CHECK(!did_handle);
    CHECK(value != 10 + v);
    did_handle = packet_handler.HandlePacket(b_packet);
    CHECK(!did_handle);
    CHECK(value != 10 * 0 + 2 * v);

    // sync
    const auto res = packet_handler.Sync(correct_packet_handler.Serialize());
    CHECK(res == PacketHandler::SyncResult::kSuccess);

    // check it is now correct
    value = 10;
    did_handle = packet_handler.HandlePacket(a_packet);
    CHECK(did_handle);
    CHECK(value == 10 + v);
    did_handle = packet_handler.HandlePacket(b_packet);
    CHECK(did_handle);
    CHECK(value == 10 * 0 + 2 * v);
  }
}
