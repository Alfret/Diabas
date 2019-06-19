#ifndef PACKET_HANDLER_HPP_
#define PACKET_HANDLER_HPP_

#include "network/packet.hpp"
#include <functional>
#include <string_view>
#include <optional>

namespace dib
{

using PacketHandlerCallback = std::function<void(const Packet&)>;

/**
 * Describes a packet type.
 * PacketHeaderType is stored separately, as key in a map.
  */
struct PacketMeta
{
  // stored as key in map
  /* PacketHeaderType type; */

  // unique name
  alflib::String name;

  // called when this packet type is received.
  PacketHandlerCallback callback;
};

/**
 * Like PacketMeta but without callback, used when serializing the data.
 */
struct PacketMetaSerializable
{
  PacketHeaderType type;
  alflib::String name;
};

// ============================================================ //

class PacketHandler
{
   // ============================================================ //
  // Lifetime
  // ============================================================ //
 public:
  PacketHandler();
  ~PacketHandler();

  // ============================================================ //
  // Core Methods
  // ============================================================ //
 public:

  /**
   * @return If the packet type was known.
   */
  bool HandlePacket(const Packet& packet) const;

  /**
   * Add a packet type with a given name. Suited for dynamically addding
   * packet types.
   * @return If we could add the packet type without name collision.
   */
  bool AddPacketType(const alflib::String& packet_type_name,
                     PacketHandlerCallback callback);

  /**
   * Add a packet type, you decide packet_type and name.
   * @param type_hint If not unique, it will be modified to be unique.
   * @return If we could add the packet type without name collision.
   */
  bool AddPacketType(PacketHeaderType type_hint,
                     const alflib::String& packet_type_name,
                     PacketHandlerCallback callback);

 public:
  enum class SyncResult {
    kSuccess = 0,

    // we have fewer packet_meta's than other
    kMissingPacketMeta,

    // we have more packet_meta's than other
    kExtraPacketMeta,

    // some packet_meta's have same hash, but different names.
    kNameMissmatch
  };

  SyncResult Sync(const std::vector<PacketMetaSerializable>& correct);

  std::vector<PacketMetaSerializable> Serialize() const;

  // ============================================================ //
  // Misc
  // ============================================================ //
 public:

  auto begin() { return packet_metas_.begin(); }

  auto end() { return packet_metas_.end(); }

  std::optional<PacketHeaderType> GetType(const alflib::String& string);

  std::size_t GetSize() const { return packet_metas_.size(); }

  PacketMeta& operator[](const PacketHeaderType& i) { return packet_metas_[i]; }

  // ============================================================ //
  // Member Variables
  // ============================================================ //

private:
  std::unordered_map<PacketHeaderType, PacketMeta> packet_metas_;
};
}

#endif//PACKET_HANDLER_HPP_
