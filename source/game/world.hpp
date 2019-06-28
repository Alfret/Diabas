#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "core/assert.hpp"
#include "network/network.hpp"
#include "network/side.hpp"
#include <string_view>
#include "game/ecs/entity_manager.hpp"
#include "game/chat/chat.hpp"

namespace dib {

class World
{
public:
  void Update();

  void OnCommandNetwork(const std::string_view input);

  void OnCommandBroadcast(const std::string_view input);

  Network<kSide>& GetNetwork() { return network_; }

  dib::EntityManager& GetEntityManager();

  game::Chat& GetChat() { return chat_; }

private:
  dib::EntityManager entity_manager_{};

  Network<kSide> network_{ this };

  game::Chat chat_{this};
};
}

#endif // WORLD_HPP_
