#ifndef PACKET_HEADER_HPP_
#define PACKET_HEADER_HPP_

#include "core/types.hpp"

namespace dib {

using PacketHeaderType = u16;

struct PacketHeader
{
  PacketHeaderType packet_type;
  u16 padding1;
};

// ============================================================ //
// Predefined packet types
// ============================================================ //

static constexpr PacketHeaderType kPacketHeaderTypeChat = 0;


}

#endif // PACKET_HEADER_HPP_
