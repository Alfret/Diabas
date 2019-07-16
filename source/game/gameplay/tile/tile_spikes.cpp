#include "tile_spikes.hpp"
#include "game/gameplay/soul.hpp"
#include "game/gameplay/moveable.hpp"
#include <dlog.hpp>

namespace dib::game
{

TileSpikes::TileSpikes(const ResourcePath& resource_path, const String& translation_key)
    : Tile(resource_path, translation_key)
{
  col_rect_.width = kTileInMeters;
  col_rect_.height = kPixelInMeter * 6;
  SetCollisionIsSolid(false);
}

bool
TileSpikes::OnCollision(World& world, const WorldPos tile_pos,
                        const entt::entity entity)
{
  (void)tile_pos;

  auto& registry = world.GetEntityManager().GetRegistry();
  auto& soul = registry.get<Soul>(entity);
  auto& moveable = registry.get<Moveable>(entity);

  constexpr s32 kDamage = -10;

  // apply damage and a force
  if (soul.ApplyDamage(kDamage)) {
    Force force{};
    force.v_vel = 10.0f;
    force.duration_s = 0.05f;
    ForceOnMoveable(moveable, force);
  }
  return true;
}

CollisionType
TileSpikes::GetCollisionType([[maybe_unused]]const World& world,
                             [[maybe_unused]]WorldPos pos) const
{
  return CollisionType::kRect;
}

Collideable
TileSpikes::GetCollideable([[maybe_unused]]const World& world,
                           [[maybe_unused]]WorldPos pos) const
{
  Collideable c{};
  CollideableRect* r = reinterpret_cast<CollideableRect*>(&c);
  r->type = CollisionType::kRect;
  r->rect = col_rect_;
  return c;
}

}
