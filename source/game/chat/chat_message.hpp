#ifndef CHAT_MESSAGE_HPP_
#define CHAT_MESSAGE_HPP_

#include "core/types.hpp"
#include "core/uuid.hpp"
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

namespace dib::game {

enum class ChatType : u8
{
  kServer = 0,
  kSay,
  kWhisper,

  /* Event must be last, used to count elements in enum */
  kEvent
};

struct ChatMessage
{
  ChatType type;
  Uuid uuid_from;
  Uuid uuid_to; // may be unused in some types
  String msg;

  // Serialize //
  bool ToBytes(alflib::RawMemoryWriter& mw) const;

  static ChatMessage FromBytes(alflib::RawMemoryReader& mr);
};
}

#endif // CHAT_MESSAGE_HPP_
