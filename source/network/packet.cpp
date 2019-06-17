#include "packet.hpp"
#include "core/assert.hpp"

namespace dib
{

Packet::Packet()
    : Packet(kDefaultPacketSize)
{
}

Packet::Packet(const std::size_t size)
    : size_(kHeaderSize)
{
  DIB_ASSERT(size >= kHeaderSize, "size must be larger or equal to header size");
  SetCapacity(size);
}

Packet::Packet(const u8* data, const std::size_t data_count)
    : Packet(data_count)
{
  SetPacket(data, data_count);
}

Packet::Packet(const char* data, const std::size_t data_count)
    : Packet(reinterpret_cast<const u8*>(data), data_count)
{}

// ============================================================ //

void
Packet::Clear()
{
  size_ = kHeaderSize;
}

void
Packet::SetCapacity(const std::size_t capacity)
{
  container_.resize(capacity);
  if (container_.capacity() > container_.size()) {
    container_.shrink_to_fit();
  }
}

bool
Packet::SetPacket(const u8* data, const std::size_t data_count)
{
  DIB_ASSERT(container_.size() >= kHeaderSize, "container too small");
  if (container_.capacity() >= data_count) {
    std::memcpy((&container_[0]), data, data_count);
    size_ = data_count;
    return true;
  } else {
    return false;
  }
}

// ============================================================ //

std::size_t
Packet::GetHeaderSize() const
{
  DIB_ASSERT(container_.size() >= kHeaderSize , "container too small");
  return kHeaderSize;
}

void
Packet::SetHeader(const PacketHeader header)
{
  std::memcpy(&container_[0], &header, kHeaderSize);
}

const PacketHeader*
Packet::GetHeader() const
{
  return reinterpret_cast<const PacketHeader*>(&container_[0]);
}

std::size_t
Packet::GetPayloadSize() const
{
  DIB_ASSERT(size_ >= kHeaderSize, "container too small");
  return size_ - kHeaderSize;
}

bool
Packet::SetPayload(const u8* data, const std::size_t data_count)
{
  DIB_ASSERT(container_.size() >= kHeaderSize, "container too small");
  if (GetBytesLeft() >= data_count) {
    std::memcpy((&container_[0]) + size_, data, data_count);
    size_ += data_count;
    return true;
  } else {
    return false;
  }
}

const u8*
Packet::GetPayload() const
{
  DIB_ASSERT(GetPayloadSize() > 0, "container too small");
  return &container_[kHeaderSize];
}

}
