#include "packet_handler.hpp"
#include "core/hash.hpp"
#include <alflib/string.hpp>
#include <alflib/assert.hpp>

namespace dib
{

PacketHandler::PacketHandler()
{

}

PacketHandler::~PacketHandler() = default;

bool
PacketHandler::AddPacketType(const alflib::String& packet_type_name,
                             CallbackType callback)
{
  const PacketHeaderType type = packet_metas_.size();
  const u64 hash = HashFNV1a(packet_type_name.GetUTF8(), packet_type_name.GetSize());
  return AddPacketType(type, hash, packet_type_name, callback);
}

bool
PacketHandler::AddPacketType(const PacketHeaderType packet_type,
                             const u64 hash,
                             const alflib::String& packet_type_name,
                             CallbackType callback)
{
  [[maybe_unused]]const auto [it, ok] =
      packet_metas_.insert({packet_type, {hash, packet_type_name, callback}});
  return ok;
}

PacketHandler::SortResult
PacketHandler::Sort(const std::vector<PacketMetaSerializable>& correct)
{
  // are we missing a packet meta?
  if (packet_metas_.size() < correct.size()) {
    return SortResult::kMissingPacketMeta;
  }
  // do we have extra packet meta?
  else if (packet_metas_.size() > correct.size()) {
    return SortResult::kExtraPacketMeta;
  }

  // are the hashes the same?
  std::size_t found = 0;
  std::size_t sorted_count = 0;
  for (const auto& correct_meta : correct) {
    for (const auto& packet_meta: packet_metas_) {
      if (correct_meta.hash == packet_meta.second.hash) {
        ++found;
        if (correct_meta.type == packet_meta.first) {
          ++sorted_count;
        }
        break;
      }
    }
  }
  if (found != packet_metas_.size()) {
    return SortResult::kHashMissmatch;
  }



  // do we need to sort?
  if (sorted_count != packet_metas_.size()) {

    // do the sorting - lazy edition
    std::unordered_map<PacketHeaderType, PacketMeta> new_packet_metas;

    for (std::size_t i = 0; i < packet_metas_.size(); i++) {
      // find the callback
      bool found = false;
      for (auto& meta : packet_metas_) {
        if (meta.second.hash == correct[i].hash) {
          AlfAssert(meta.second.name == correct[i].name,
                    "names should match");
          found = true;
          [[maybe_unused]] const auto [it, ok] = new_packet_metas.insert(
            { correct[i].type,
              { correct[i].hash, correct[i].name, meta.second.callback} });
          AlfAssert(ok, "could not insert into map");
          break;
        }
      }
      AlfAssert(found, "could not find hash, but previous code guarantees it");
    }

    packet_metas_ = new_packet_metas;
  }

  return SortResult::kSuccess;
}

u64
PacketHandler::GetHash(const alflib::String& string)
{
  return HashFNV1a(string.GetUTF8(), string.GetSize());
}
}
