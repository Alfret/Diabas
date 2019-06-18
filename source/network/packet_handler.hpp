#ifndef PACKET_HANDLER_HPP_
#define PACKET_HANDLER_HPP_

#include "network/packet.hpp"
#include <functional>
#include <string_view>

namespace dib
{

using CallbackType = std::function<void(const Packet&)>;

/**
 * PacketHeaderType is stored separate.
 */
struct PacketMeta
{
  u64 hash;
  alflib::String name;
  CallbackType callback;
};

/**
 * To be used when sending over network
 */
struct PacketMetaSerializable
{
  PacketHeaderType type;
  u64 hash;
  alflib::String name;
};

// ============================================================ //

class PacketHandler
{
 public:

  PacketHandler();
  ~PacketHandler();

  /**
   * Add a packet type that is dynamically loaded.
   */
  bool AddPacketType(const alflib::String& packet_type_name,
                     CallbackType callback);

  /**
   * Add a packet type that is known at compile time.
   */
  bool AddPacketType(const PacketHeaderType packet_type,
                     const u64 hash,
                     const alflib::String& packet_type_name,
                     CallbackType callback);

  enum class SortResult {
    kSuccess = 0,

    // we have fewer packet_meta's than other
    kMissingPacketMeta,

    // we have more packet_meta's than other
    kExtraPacketMeta,

    // some packet_meta's have different hash.
    kHashMissmatch
  };

  /**
   * Sort the PacketMeta's, such that our packet_type's are consistent
   * with the correct packet_type's. This is important because the
   * right callback needs to be paired with the right packet_type.
   */
  SortResult Sort(const std::vector<PacketMetaSerializable>& correct);

  auto begin() { return packet_metas_.begin(); }

  auto end() { return packet_metas_.end(); }

  static u64 GetHash(const alflib::String& string);

  std::size_t GetSize() const { return packet_metas_.size(); }

  PacketMeta& operator[](const PacketHeaderType& i) { return packet_metas_[i]; }

private:
  std::unordered_map<PacketHeaderType, PacketMeta> packet_metas_;
};
}

#endif//PACKET_HANDLER_HPP_
