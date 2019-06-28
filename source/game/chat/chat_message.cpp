#include "chat_message.hpp"

namespace dib::game {

bool
ChatMessage::ToBytes(alflib::RawMemoryWriter& mw) const
{
  mw.Write(static_cast<u8>(type));
  mw.Write(uuid_from);
  mw.Write(uuid_to);
  return mw.Write(msg);
}

ChatMessage
ChatMessage::FromBytes(alflib::RawMemoryReader& mr)
{
  ChatMessage msg{};
  msg.type = static_cast<ChatType>(*(mr.ReadBytes(1)));
  msg.uuid_from = mr.Read<Uuid>();
  msg.uuid_to = mr.Read<Uuid>();
  msg.msg = mr.Read<String>();
  return msg;
}
}
