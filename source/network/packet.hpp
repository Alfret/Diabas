#ifndef PACKET_HPP_
#define PACKET_HPP_

#include "core/types.hpp"
#include "network/packet_header.hpp"
#include <alflib/string.hpp>
#include <vector>

namespace dib {

using ContainerValueType = u8;

struct PayloadIterator
{
  /**
   * Use to construct an iterator pointing to the start of the payload.
   * @param payload Pointer to the payload
   */
  explicit PayloadIterator(ContainerValueType* payload);

  /**
   * To construct a end iterator, set pos to one step beyond the end of the
   * payload.
   * @param payload Pointer to the payload
   * @param pos Where in the payload we are.
   */
  PayloadIterator(ContainerValueType* payload, const std::size_t pos);

  bool operator!=(const PayloadIterator& other) const;

  const ContainerValueType& operator*() const { return payload_[pos_]; }

  ContainerValueType& operator*() { return payload_[pos_]; }

  PayloadIterator& operator++();

  PayloadIterator& operator--();

private:
  std::size_t pos_;
  ContainerValueType* payload_;
};

/**
 * A packet consists of a header and a payload, stored in contiguous memory.
 */
class Packet
{
public:
  using ValueType = ContainerValueType;

  // ============================================================ //
  // Lifetime
  // ============================================================ //
public:
  Packet();

  /**
   * Construct a packet.
   * @param size How large payload size to allocate
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
  std::size_t GetBytesLeft() const
  {
    return GetPacketCapacity() - GetPacketSize();
  }

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

  alflib::String ToString() const;

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
