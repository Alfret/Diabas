#ifndef PACKET_HANDLER_HPP_
#define PACKET_HANDLER_HPP_

#include "network/packet.hpp"
#include <functional>
#include <string_view>
#include <optional>
#include <array>

namespace dib
{

// ============================================================ //
// Predefined packet types
// ============================================================ //

enum class PacketHeaderStaticTypes : std::size_t
{
  kSync = 0,

  // Must be last, used to count number of constants in the enum
  kChat
};

constexpr std::size_t kPacketHeaderStaticTypesCount =
    static_cast<std::size_t>(PacketHeaderStaticTypes::kChat) + 1;

// ============================================================ //
// Helper Types
// ============================================================ //

using PacketHandlerCallback = std::function<void(const Packet&)>;

/**
 * Describes a packet type and how to handle it.
 * PacketHeaderType is stored separately, as key in a map.
 */
struct PacketMeta
{
  // stored as key in map
  /* PacketHeaderType type; */

  // unique name
  String name;

  // called when this packet type is received.
  PacketHandlerCallback callback;
};

/**
 * Like PacketMeta but without callback, used when serializing the data.
 */
struct PacketMetaSerializable
{
  PacketHeaderType type;
  String name;
};

// ============================================================ //
// Packet Handler
// ============================================================ //

/**
 * Packet handler maps a packet type with a callback function.
 */
class PacketHandler
{
 public:
  PacketHandler();
  ~PacketHandler();

  // ============================================================ //
  // Packet Consumer
  // ============================================================ //
 public:

  /**
   * @return If the packet type was known.
   */
  bool HandlePacket(const Packet& packet) const;

  /**
   *
   * @param packet_type_name Must be unique.
   * @return If name is not unique, the add fails and returns false
   */
  bool AddDynamicPacketType(const String& packet_type_name,
                            PacketHandlerCallback callback);

  /**
   *
   * @param packet_type_name Must be unique.
   * @return If name is not unique, the add fails and returns false
   */
  bool AddStaticPacketType(const PacketHeaderStaticTypes static_type,
                           const String& packet_type_name,
                           PacketHandlerCallback callback);

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

  // ============================================================ //

  // used in test, never use this yourself
  bool UnsafeAddDynamicPacketType(const String& packet_type_name,
                                  const PacketHeaderType type_hint,
                                  PacketHandlerCallback callback);
 private:
   bool AddDynamicPacketTypeBase(const String& packet_type_name,
                                 PacketHeaderType type_hint,
                                 PacketHandlerCallback callback);

   // ============================================================ //
   // Packet Producer
   // ============================================================ //
 public:
  /**
   * Make a header of static packet type, and fill it in for the packet.
   */
  void BuildPacketHeader(Packet& packet,
                       const PacketHeaderStaticTypes static_type) const;

  /**
   * Make a header of dynamic packet type, and fill it in for the packet.
   * @return If we can find a packet type for the given name.
   */
  bool BuildPacketHeader(Packet& packet, const String& name) const;

 private:
  /**
   * Prepare the packet header.
   */
  void BuildPacketHeader(Packet& packet, const PacketHeaderType type) const;

  // ============================================================ //
  // Misc
  // ============================================================ //
 public:
  std::vector<PacketMetaSerializable> Serialize() const;

  /**
   * Search for the packet type with the given name, among the packet types
   * that are dynamic.
   */
  std::optional<PacketHeaderType> FindDynamicType(const String& name) const;

  auto begin() { return packet_metas_.begin(); }

  auto end() { return packet_metas_.end(); }

  std::size_t GetSize() const { return packet_metas_.size(); }

  PacketMeta& operator[](const PacketHeaderType& i) { return packet_metas_[i]; }

  // ============================================================ //
  // Member Variables
  // ============================================================ //

 private:
  std::unordered_map<PacketHeaderType, PacketMeta> packet_metas_;

  /**
   * The PacketHeaderStaticTypes points to an index in this array.
   */
  std::array<PacketHeaderType, kPacketHeaderStaticTypesCount> static_types_{};

  std::unordered_map<String, PacketHeaderType> dynamic_types_{};
};
}

#endif//PACKET_HANDLER_HPP_
