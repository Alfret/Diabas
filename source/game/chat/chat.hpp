#ifndef CHAT_HPP_
#define CHAT_HPP_

#include "core/types.hpp"
#include "game/chat/chat_message.hpp"
#include <dutil/queue.hpp>

namespace dib::game {

class World;

class Chat
{
public:
  Chat(World* world);

  /**
   * Send a chat message to server, will not add it locally until
   * we get it back from server.
   * @return If the data filled out in msg is valid.
   */
  bool SendMessage(ChatMessage msg) const;

  /**
   * Add a chat message to our chat, will not validate
   */
  void ParseMessage(ChatMessage&& msg);

  String GetDebug() const { return debug_; };

  /**
   * Check if the message is valid. Behaves differently on server and client.
   */
  bool ValidateMessage(const ChatMessage& msg) const;

private:
  void Debug();

private:
  dutil::Queue<ChatMessage, dutil::kNoMutex> messages_{ 10 };
  String debug_{};
  World* world_;
};

}

#endif // CHAT_HPP_
