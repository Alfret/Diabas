#include "main.test.hpp"
#include "network/packet.hpp"

using namespace dib;

TEST_SUITE("packet")
{
  TEST_CASE("basic")
  {
    Packet packet{};

    PacketHeader header{};
    header.a = 1;
    header.b = 2;
    header.c = 3;
    header.d = 4;

    packet.SetHeader(header);

    CHECK(std::memcmp(&header, packet.GetHeader(), packet.GetHeaderSize()) ==
          0);
    CHECK(packet.GetPacketSize() == sizeof(header));
    CHECK(packet.GetHeaderSize() == sizeof(header));

    enum { buf_size = 5};
    u8 buf[buf_size] = {2,4,8,16,32};
    packet.SetPayload(buf, buf_size);

    CHECK(std::memcmp(buf, packet.GetPayload(), buf_size) == 0);
    CHECK(packet.GetPayloadSize() == buf_size);
    CHECK(packet.GetPacketSize() == sizeof(header) + buf_size);

    packet.Clear();
    CHECK(packet.GetPayloadSize() == 0);
  }

  TEST_CASE("string")
  {
    alflib::String str = "this is a string";
    Packet packet(str);

    CHECK(str.GetSize() == packet.GetPacketSize() - packet.GetHeaderSize());
    CHECK(std::memcmp(str.GetUTF8(), packet.GetPayload(), str.GetSize()) == 0);
    alflib::String packet_str = packet.ToString();
    CHECK(str == packet_str);
  }
}
