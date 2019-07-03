#include "chat_message.hpp"

namespace dib::game {

bool
ChatMessage::ToBytes(alflib::RawMemoryWriter& mw) const
{
  mw.Write(static_cast<u8>(type));
  mw.Write(uuid_from);
  mw.Write(uuid_to);
  mw.Write(msg);
  mw.Write(from);
  return mw.Write(to);
}

ChatMessage
ChatMessage::FromBytes(alflib::RawMemoryReader& mr)
{
  ChatMessage msg{};
  msg.type = static_cast<ChatType>(*(mr.ReadBytes(1)));
  msg.uuid_from = mr.Read<Uuid>();
  msg.uuid_to = mr.Read<Uuid>();
  msg.msg = mr.Read<String>();
  msg.from = mr.Read<String>();
  msg.to = mr.Read<String>();
  return msg;
}
}
