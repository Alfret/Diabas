#include "chat_message.hpp"

namespace dib::game {

void
ChatMessage::ToBytes(alflib::MemoryWriter& mw) const
{
  mw.Write(static_cast<u8>(type));
  mw.Write(uuid_from);
  mw.Write(uuid_to);
  mw.Write(msg);
}

ChatMessage
ChatMessage::FromBytes(alflib::MemoryReader& mr)
{
  ChatMessage msg{};
  msg.type = mr.Read<ChatType>();
  msg.uuid_from = mr.Read<Uuid>();
  msg.uuid_to = mr.Read<Uuid>();
  msg.msg = mr.Read<String>();
  return msg;
}
}
