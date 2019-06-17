#ifndef PACKET_HPP_
#define PACKET_HPP_

#include "core/types.hpp"
#include <vector>
#include "network/packet_header.hpp"

namespace dib {

/**
 * A packet consists of a header and a payload, stored in contiguous memory.
 */
class Packet
{

  // ============================================================ //
  // Lifetime
  // ============================================================ //
 public:
  Packet();

  explicit Packet(const std::size_t size);

  Packet(const u8* data, const std::size_t data_count);

  Packet(const char* data, const std::size_t data_count);

  // ============================================================ //
  // Packet / General
  // ============================================================ //
 public:

  /**
   * Clear the packet.
   */
  void Clear();

  /**
   * Get how many bytes are unused in the container.
   */
  std::size_t GetBytesLeft() const { return GetCapacity() - size_; }

  /**
   * Set amount of bytes allocated for container. Capacity includes both
   * header and payload.
   */
  void SetCapacity(const std::size_t capacity);

  std::size_t GetCapacity() const { return container_.size(); }

  std::size_t GetPacketSize() const { return GetHeaderSize() + GetPayloadSize(); }

  const u8* GetPacket() const { return &container_[0]; }

  /**
   * Simply take a data source, and write that into our container.
   */
  bool SetPacket(const u8* data, const std::size_t data_count);

  // ============================================================ //
  // Header
  // ============================================================ //
public:
  std::size_t GetHeaderSize() const;

  void SetHeader(const PacketHeader header);

  const PacketHeader* GetHeader() const;

  // ============================================================ //
  // Payload
  // ============================================================ //
 public:
  std::size_t GetPayloadSize() const;

  /**
   * @return If we could write the entire data into payload.
   */
  bool SetPayload(const u8* data, const std::size_t data_count);

  const u8* GetPayload() const;

  // ============================================================ //
  // Constants
  // ============================================================ //
 private:
  static constexpr std::size_t kHeaderSize = sizeof(PacketHeader);
  static constexpr std::size_t kDefaultPacketSize = 1024;

  // ============================================================ //
  // Member variables
  // ============================================================ //
  using PacketContainer = std::vector<u8>;

  std::size_t size_;
  PacketContainer container_{};
};

}

#endif // PACKET_HPP_
