#include "packet.hpp"
#include "core/assert.hpp"
#include <dlog.hpp>

namespace dib
{

struct PayloadIterator
{
  /**
   * Use to construct an iterator pointing to the start of the payload.
   * @param payload Pointer to the payload
   */
  explicit PayloadIterator(Packet::ValueType* payload)
      : PayloadIterator(payload, 0) {}

  /**
   * To construct a end iterator, set pos to one step beyond the end of the
   * payload.
   * @param payload Pointer to the payload
   * @param pos Where in the payload we are.
   */
  PayloadIterator(Packet::ValueType* payload, const std::size_t pos)
      : pos_(pos)
      , payload_(payload)
  {}

  bool operator!=(const PayloadIterator& other) const {
    return pos_ != other.pos_;
  }

  const Packet::ValueType& operator*() const {
    return payload_[pos_];
  }

  Packet::ValueType& operator*() {
    return payload_[pos_];
  }

  PayloadIterator& operator++() {
    ++pos_;
    return *this;
  }

  PayloadIterator& operator--()
  {
    --pos_;
    return *this;
  }

 private:
  std::size_t pos_;
  Packet::ValueType* payload_;
};

// ============================================================ //

Packet::Packet()
    : Packet(kDefaultPacketSize)
{
}

Packet::Packet(const std::size_t size)
    : size_(kHeaderSize)
{
  DIB_ASSERT(size >= kHeaderSize, "size must be larger or equal to header size");
  SetPacketCapacity(size);
}

Packet::Packet(const u8* data, const std::size_t data_count)
    : Packet(data_count)
{
  SetPacket(data, data_count);
}

Packet::Packet(const char8* data, const std::size_t data_count)
  : Packet(data_count + GetHeaderSize())
{
  ClearHeader();
  SetPayload(data, data_count);
}

Packet::Packet(const alflib::String& string)
    : Packet(string.GetSize() + GetHeaderSize())
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
  DIB_ASSERT(container_.capacity() >= kHeaderSize , "container too small");
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

const u8* Packet::GetPayload() const
{
  DIB_ASSERT(GetPayloadSize() > 0, "container too small");
  return &container_[kHeaderSize];
}

u8* Packet::GetPayload()
{
  DIB_ASSERT(GetPayloadSize() > 0, "container too small");
  return &container_[kHeaderSize];
}

PayloadIterator Packet::begin() { return PayloadIterator{ GetPayload() }; }

PayloadIterator Packet::end()
{
  return PayloadIterator{ GetPayload(), GetPayloadCapacity() };
}

alflib::String Packet::ToString()
{
  // TODO when alflib is updated remove the null termination.
  if (size_ == GetPayloadCapacity()) {
    DLOG_ERROR("could not construct string from buffer since it is full.");
    return alflib::String{};
  }

  container_[size_] = 0;
  alflib::String str{ reinterpret_cast<const char8*>(GetPayload()) };
  return str;
}
}
