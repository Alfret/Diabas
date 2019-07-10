#ifndef WORLD_HPP_
#define WORLD_HPP_

// ========================================================================== //
// Headers
// ========================================================================== //

#include <string_view>

#include "core/assert.hpp"
#include "network/network.hpp"
#include "network/side.hpp"
#include "game/ecs/entity_manager.hpp"
#include "game/terrain.hpp"
#include "game/chat/chat.hpp"
#include "game/tile/tile_registry.hpp"

// ========================================================================== //
// World Declaration
// ========================================================================== //

namespace dib::game {

/** Class representing the game world **/
class World
{
public:
  World();

  void Update();

  void OnCommandNetwork(const std::string_view input);

  void OnCommandBroadcast(const std::string_view input);

  Network<kSide>& GetNetwork() { return network_; }

  /** Returns the terrain of the world **/
  Terrain& GetTerrain() { return mTerrain; }

  /** Returns the terrain of the world **/
  const Terrain& GetTerrain() const { return mTerrain; }

  /** Returns the entity manager **/
  dib::EntityManager& GetEntityManager();

  game::Chat& GetChat() { return chat_; }

private:
  /** Terrain **/
  Terrain mTerrain;

  dib::EntityManager entity_manager_{};

  Network<kSide> network_{ this };

  game::Chat chat_{ this };
};
}

#endif // WORLD_HPP_
