#include "tile_spikes.hpp"
#include "game/gameplay/soul.hpp"
#include "game/gameplay/moveable.hpp"

namespace dib::game
{

TileSpikes::TileSpikes(const ResourcePath& resource_path, const String& translation_key)
    : Tile(resource_path, translation_key)
{}

void
TileSpikes::OnCollision(World& world, const WorldPos tile_pos, const entt::entity entity)
{
  auto& registry = world.GetEntityManager().GetRegistry();
  auto& soul = registry.get<Soul>(entity);
  auto& moveable = registry.get<Moveable>(entity);

  constexpr s32 kDamage = -10;

  // apply damage and a force
  if (soul.ApplyDamage(kDamage)) {
    ForceOnMoveable(moveable, 0.0f, 10.0f);
  }
}

}
