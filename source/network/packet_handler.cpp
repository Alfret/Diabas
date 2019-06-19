#include "packet_handler.hpp"
#include "core/hash.hpp"
#include <alflib/string.hpp>
#include <alflib/assert.hpp>
#include <dlog.hpp>

namespace dib
{

PacketHandler::PacketHandler() = default;

PacketHandler::~PacketHandler() = default;

bool
PacketHandler::HandlePacket(const Packet& packet) const
{
  bool could_handle = false;

  const PacketHeader* header = packet.GetHeader();
  auto it = packet_metas_.find(header->type);
  if (it != packet_metas_.end()) {
    could_handle = true;
    // TODO maybe not send the header, or clear the header first???
    it->second.callback(packet);
  } else {
    DLOG_WARNING("got packet of unknown type [{}]", header->type);
  }

  return could_handle;
}

bool
PacketHandler::AddPacketType(const alflib::String& packet_type_name,
                             PacketHandlerCallback callback)
{
  const PacketHeaderType type =
    HashFNV1a32(packet_type_name.GetUTF8(), packet_type_name.GetSize());

  return AddPacketType(type, packet_type_name, callback);
}

bool
PacketHandler::AddPacketType(PacketHeaderType type_hint,
                             const alflib::String& packet_type_name,
                             PacketHandlerCallback callback)
{
  for (const auto it : packet_metas_) {
    if (it.second.name == packet_type_name) {
      DLOG_ERROR("could not add packet type for [{}] since it already exist"
                 " an entry with that name.", packet_type_name);
      return false;
    }
  }

  // ensure no collision
  for (;;) {
    if (packet_metas_.find(type_hint) == packet_metas_.end()) {
      break;
    }
    type_hint += 1;
  }

  [[maybe_unused]] const auto [it, ok] =
    packet_metas_.insert({ type_hint, { packet_type_name, callback } });
  if (!ok) {
    DLOG_ERROR("could not add packet type [{}] because the hash was not "
               "unique, [{}].", packet_type_name, type_hint);
  }
  return ok;
}

PacketHandler::SyncResult
PacketHandler::Sync(const std::vector<PacketMetaSerializable>& correct)
{
  // are we missing a packet meta?
  if (packet_metas_.size() < correct.size()) {
    return SyncResult::kMissingPacketMeta;
  }
  // do we have extra packet meta?
  else if (packet_metas_.size() > correct.size()) {
    return SyncResult::kExtraPacketMeta;
  }

  // for each name, are the hashes correct?
  std::size_t found_name = 0;
  std::size_t correct_hash = 0;
  for (const auto& correct_meta : correct) {
    for (const auto& meta : packet_metas_) {
      if (correct_meta.name == meta.second.name) {
        found_name++;
        if (correct_meta.type == meta.first) {
          ++correct_hash;
        }
      }
    }
  }
  if (found_name != packet_metas_.size()) {
    return SyncResult::kNameMissmatch;
  }

  // if names are same, but hash does not match, correct the hashes
  if (correct_hash != packet_metas_.size()) {
    std::unordered_map<PacketHeaderType, PacketMeta> new_packet_metas;

    for (const auto& correct_meta : correct) {
      bool found = true;
      for (const auto& meta : packet_metas_) {

        if (meta.second.name == correct_meta.name) {
          [[maybe_unused]] const auto [it, ok] = new_packet_metas.insert(
            { correct_meta.type, { correct_meta.name, meta.second.callback } });
          AlfAssert(ok, "could not insert key for packet type [{}]",
                    correct_meta.name);
        }
      }
      AlfAssert(found, "could not find name, but should be guaranteed");
    }

    packet_metas_ = new_packet_metas;
  }

  return SyncResult::kSuccess;
}

std::vector<PacketMetaSerializable>
PacketHandler::Serialize() const
{
  std::vector<PacketMetaSerializable> v{};
  for (const auto it : packet_metas_) {
    v.push_back({it.first, it.second.name});
  }
  return v;
}

std::optional<PacketHeaderType>
PacketHandler::GetType(const alflib::String& string)
{
  for (const auto it : packet_metas_) {
    if (string == it.second.name) {
      return std::optional<PacketHeaderType>{ it.first };
    }
  }
  return std::nullopt;
}
}
