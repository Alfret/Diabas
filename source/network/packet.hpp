#ifndef PACKET_HPP_
#define PACKET_HPP_

#include "core/types.hpp"
#include <vector>
#include "network/packet_header.hpp"
#include <alflib/string.hpp>

namespace dib {

// forward declaration
struct PayloadIterator;

/**
 * A packet consists of a header and a payload, stored in contiguous memory.
 */
class Packet
{
  public:
  using ValueType = u8;

  // ============================================================ //
  // Lifetime
  // ============================================================ //
 public:
  Packet();

  /**
   * Construct a packet with given size.
   */
  explicit Packet(const std::size_t size);

  /**
   * Copy data into a new packet, starting from header
   * @param data_count How many bytes to copy over. Must be more than
   * GetHeaderSize().
   */
  Packet(const ValueType* data, const std::size_t data_count);

  /**
   * Copy data into payload, will write 0's to header.
   */
  Packet(const char8* data, const std::size_t data_count);

  /**
   * Write string into payload, and write 0's to header
   */
  Packet(const alflib::String& string);

  // ============================================================ //
  // Packet / General
  // ============================================================ //
 public:

  /**
   * Clear the packet, will not write anything to the container.
   */
  void Clear();

  /**
   * Get how many bytes are unused in the packet.
   */
  std::size_t GetBytesLeft() const { return GetPacketCapacity() - GetPacketSize(); }

  /**
   * Set amount of bytes allocated for container. Includes both
   * header and payload.
   */
  void SetPacketCapacity(const std::size_t capacity);

  /**
   * Get total bytes allocated by packet.
   */
  std::size_t GetPacketCapacity() const;

  /**
   * Get total bytes used by packet.
   */
  std::size_t GetPacketSize() const;

  const ValueType* GetPacket() const { return &container_[0]; }

 private:
  ValueType* GetPacket() { return &container_[0]; }

 public:
  /**
   * Simply take a data source, and write that into our container.
   * @return False if data_count is too large for our packet.
   */
  bool SetPacket(const ValueType* data, const std::size_t data_count);

  // ============================================================ //
  // Header
  // ============================================================ //
public:
  std::size_t GetHeaderSize() const;

  void SetHeader(const PacketHeader header);

  const PacketHeader* GetHeader() const;

 private:
  ValueType* GetHeaderRaw();

 public:
  /**
   * Write 0's to header;
   */
  void ClearHeader();

  // ============================================================ //
  // Payload
  // ============================================================ //
 public:
  /**
   * Get used bytes by the payload.
   */
  std::size_t GetPayloadSize() const;

  /**
   * Get the total amount of bytes that can be used by the payload.
   */
  std::size_t GetPayloadCapacity() const;

  /**
   * @return If we could write the entire data into payload.
   */
  bool SetPayload(const ValueType* data, const std::size_t data_count);
  bool SetPayload(const char8* str, const std::size_t str_len);

  const ValueType* GetPayload() const;

  ValueType* GetPayload();

  PayloadIterator begin();

  PayloadIterator end();

  alflib::String ToString();

  // ============================================================ //
  // Constants
  // ============================================================ //
 private:
  static constexpr std::size_t kHeaderSize = sizeof(PacketHeader);
  static constexpr std::size_t kDefaultPacketSize = 1024;

  // ============================================================ //
  // Member variables
  // ============================================================ //
 private:
  using PacketContainer = std::vector<ValueType>;

  std::size_t size_;

  PacketContainer container_{};
};

}

#endif // PACKET_HPP_
