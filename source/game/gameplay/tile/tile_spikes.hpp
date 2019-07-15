#ifndef TILE_SPIKES_HPP_
#define TILE_SPIKES_HPP_

#include "game/tile/tile.hpp"
#include <entt/entt.hpp>
#include "game/world.hpp"

namespace dib::game
{
class TileSpikes : public Tile
{
 public:
  TileSpikes(const ResourcePath& resource_path, const String& translation_key);
  void OnCollision(World& world, WorldPos tile_pos, entt::entity entity) override;

 private:

};
}

#endif//TILE_SPIKES_HPP_
