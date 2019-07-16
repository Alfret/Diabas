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
  bool OnCollision(World& world, WorldPos tile_pos, Entity entity) override;

  CollisionType GetCollisionType(const World& world, WorldPos pos) const override;

  Collideable GetCollideable(const World& world, WorldPos pos) const override;

 private:
  CollisionRectNoOffset col_rect_{};
};



}

#endif//TILE_SPIKES_HPP_
