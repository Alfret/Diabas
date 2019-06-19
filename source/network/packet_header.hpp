#ifndef PACKET_HEADER_HPP_
#define PACKET_HEADER_HPP_

#include "core/types.hpp"

namespace dib {

using PacketHeaderType = u32;

struct PacketHeader
{
  PacketHeaderType type;
};

// ============================================================ //
// Predefined packet types
// ============================================================ //

constexpr PacketHeaderType kPacketHeaderTypeChat = 0;

}

#endif // PACKET_HEADER_HPP_
