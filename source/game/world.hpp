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

  TileManager& GetTileManager() { return mTileManager; }

  const TileManager& GetTileManager() const { return mTileManager; }

  Terrain& GetTerrain() { return mTerrain; }

  const Terrain& GetTerrain() const { return mTerrain; }

  dib::EntityManager& GetEntityManager();

private:
  /** Tile manager **/
  TileManager mTileManager;
  /** Terrain **/
  Terrain mTerrain;

  dib::EntityManager entity_manager_{};
  Network<kSide> network_{ this };
};
}

#endif // WORLD_HPP_
