#ifndef WORLD_HPP_
#define WORLD_HPP_

// ========================================================================== //
// Headers
// ========================================================================== //

#include <string_view>
#include <alflib/memory/memory_reader.hpp>
#include <alflib/memory/memory_writer.hpp>

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

  World(World&& other);

  World& operator=(World&& other);

  void Update(f64 delta);

  void OnCommandNetwork(const std::string_view input);

  void OnCommandBroadcast(const std::string_view input);

  Network<kSide>& GetNetwork() { return network_; }

  /** Save world to path **/
  bool Save(const Path& path, bool overwrite = false);

  /** Load world from path **/
  bool Load(const Path& path);

  /** Load world from memory reader **/
  bool Load(alflib::MemoryReader& reader);

  /** Returns the terrain of the world **/
  Terrain& GetTerrain() { return mTerrain; }

  /** Returns the terrain of the world **/
  const Terrain& GetTerrain() const { return mTerrain; }

  /** Returns the entity manager **/
  dib::EntityManager& GetEntityManager();

  game::Chat& GetChat() { return chat_; }

  bool ToBytes(alflib::MemoryWriter& writer) const;

public:
  static World FromBytes(alflib::MemoryReader& reader);

private:
  /** Terrain **/
  Terrain mTerrain;

  dib::EntityManager entity_manager_{};

  Network<kSide> network_{ this };

  game::Chat chat_{ this };
};
}

#endif // WORLD_HPP_
