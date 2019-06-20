#include "packet.hpp"
#include "core/assert.hpp"
#include <dlog.hpp>

namespace dib {

PayloadIterator::PayloadIterator(ContainerValueType* payload)
    : PayloadIterator(payload, 0)
{}

PayloadIterator::PayloadIterator(ContainerValueType* payload, const std::size_t pos)
  : pos_(pos)
  , payload_(payload)
{}

bool
PayloadIterator::operator!=(const PayloadIterator& other) const
{
  return pos_ != other.pos_;
}

PayloadIterator&
PayloadIterator::operator++()
{
  ++pos_;
  return *this;
}

PayloadIterator&
PayloadIterator::operator--()
{
  --pos_;
  return *this;
}

// ============================================================ //

Packet::Packet()
  : Packet(kDefaultPacketSize)
{}

Packet::Packet(const std::size_t size)
  : size_(kHeaderSize)
{
  SetPacketCapacity(kHeaderSize + size);
}

Packet::Packet(const u8* data, const std::size_t data_count)
    : size_(data_count)
{
  DIB_ASSERT(data_count >= kHeaderSize,
             "data_count must be larger or equal to header size");
  SetPacketCapacity(data_count);
  SetPacket(data, data_count);
}

Packet::Packet(const char8* data, const std::size_t data_count)
  : Packet(data_count)
{
  ClearHeader();
  SetPayload(data, data_count);
}

Packet::Packet(const alflib::String& string)
  : Packet(string.GetSize())
{
  ClearHeader();
  SetPayload(string.GetUTF8(), string.GetSize());
}

// ============================================================ //

void
Packet::Clear()
{
  size_ = kHeaderSize;
}

void
Packet::SetPacketCapacity(const std::size_t capacity)
{
  container_.resize(capacity);
  if (container_.capacity() > container_.size()) {
    container_.shrink_to_fit();
  }

  // shrink_to_fit may be ignored by some implementations.
  // So make sure we use all bytes allocated.
  if (container_.capacity() > container_.size()) {
    container_.resize(container_.capacity());
  }
}

std::size_t
Packet::GetPacketCapacity() const
{
  return container_.capacity();
}

std::size_t
Packet::GetPacketSize() const
{
  return size_;
}

bool
Packet::SetPacket(const u8* data, const std::size_t data_count)
{
  DIB_ASSERT(container_.capacity() >= kHeaderSize, "container too small");
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
  DIB_ASSERT(container_.capacity() >= kHeaderSize, "container too small");
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

Packet::ValueType*
Packet::GetHeaderRaw()
{
  return GetPacket();
}

void
Packet::ClearHeader()
{
  std::memset(GetHeaderRaw(), 0, GetHeaderSize());
}

std::size_t
Packet::GetPayloadSize() const
{
  DIB_ASSERT(size_ >= kHeaderSize, "container too small");
  DIB_ASSERT(container_.capacity() >= kHeaderSize, "container too small");
  return size_ - kHeaderSize;
}

std::size_t
Packet::GetPayloadCapacity() const
{
  DIB_ASSERT(size_ >= kHeaderSize, "container too small");
  DIB_ASSERT(container_.capacity() >= kHeaderSize, "container too small");
  return GetPacketCapacity() - kHeaderSize;
}

bool
Packet::SetPayload(const u8* data, const std::size_t data_count)
{
  DIB_ASSERT(container_.capacity() >= kHeaderSize, "container too small");
  if (GetBytesLeft() >= data_count) {
    std::memcpy((&container_[0]) + size_, data, data_count);
    size_ += data_count;
    return true;
  } else {
    return false;
  }
}

bool
Packet::SetPayload(const char8* str, const std::size_t str_len)
{
  return SetPayload(reinterpret_cast<const u8*>(str), str_len);
}

const u8*
Packet::GetPayload() const
{
  DIB_ASSERT(GetPayloadSize() > 0, "container too small");
  return &container_[kHeaderSize];
}

u8*
Packet::GetPayload()
{
  DIB_ASSERT(GetPayloadSize() > 0, "container too small");
  return &container_[kHeaderSize];
}

PayloadIterator
Packet::begin()
{
  return PayloadIterator{ GetPayload() };
}

PayloadIterator
Packet::end()
{
  return PayloadIterator{ GetPayload(), GetPayloadCapacity() };
}

#include <string>
alflib::String
Packet::ToString() const
{
  // TODO when alflib is updated remove the std::string
  if (GetPayloadSize() > 0) {
    std::string stdstr{reinterpret_cast<const char8*>(GetPayload()), GetPayloadSize()};
    alflib::String str{ stdstr.c_str() };
    return str;
  }
  return "";
}
}
