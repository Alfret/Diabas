#ifndef CHAT_HPP_
#define CHAT_HPP_

#include "core/types.hpp"
#include "game/chat/chat_message.hpp"
#include <dutil/queue.hpp>

namespace dib {
class World;
}

namespace dib::game
{

class Chat
{
 public:
  /**
   * Send a chat message to server, will not add it locally until
   * we get it back from server.
   * @return If the data filled out in msg is valid.
   */
  bool SendMessage(ChatMessage msg) const;

  String GetDebug() const { return debug_; };

private:
  void Add(ChatMessage msg);

  void Debug();

private:
  dutil::Queue<ChatMessage, dutil::kNoMutex> messages_{10};
  String debug_{};
  World* world_;
};

}

#endif//CHAT_HPP_
