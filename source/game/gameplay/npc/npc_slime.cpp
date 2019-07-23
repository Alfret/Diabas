#include "npc_slime.hpp"
#include <dlog.hpp>
#include "game/npc/npc_registry.hpp"
#include "game/world.hpp"
#include <optional>

namespace dib::game {

void
NpcSlime::Update(World& world, f64 delta)
{
  auto& moveable =
    world.GetEntityManager().GetRegistry().get<Moveable>(entity_);

  const auto FindClosestPlayer =
    [&world](Position source) -> std::optional<Position> {
    auto view =
      world.GetEntityManager().GetRegistry().view<PlayerData, Moveable>();
    Position target{ 99999999999.0f, 99999999999.0f };
    for (const auto entity : view) {
      if (Distance(view.get<Moveable>(entity).position, source) <
          Distance(target, source)) {
        target = view.get<Moveable>(entity).position;
      }
    }
    if (Distance(target, source) < 100.0f) {
      return target;
    }
    return std::nullopt;
  };

  if (auto maybe_pos = FindClosestPlayer(moveable.position); maybe_pos) {
    brain.Think(world, static_cast<f32>(delta), moveable, *maybe_pos);
  }
}

void
NpcSlime::OnSpawn(World&)
{
  DLOG_VERBOSE("I spawned!");
}

void
NpcSlime::OnDeath(World&)
{
  DLOG_VERBOSE("I died!");
}

bool
NpcSlime::Store(const EntityManager& em, alflib::RawMemoryWriter& mw) const
{
  return Npc::Store(em, mw);
}

void
NpcSlime::Load(EntityManager& em, alflib::RawMemoryReader& mr)
{
  Npc::Load(em, mr);
}

Npc*
SlimeFactory(EntityManager& em, NpcID id, NpcType type)
{
  Moveable m{};
  m.velocity_input = 10.0f;
  m.velocity_max = 10.0f;
  m.velocity_jump = 10.0f;
  m.position = Position(TileToMeter(10), TileToMeter(13));
  m.width = 32;
  m.height = 48;
  Collideable c{};
  CollideableRect* cr = reinterpret_cast<CollideableRect*>(&c);
  cr->type = CollisionType::kRect;
  cr->rect = CollisionRectNoOffset{ PixelToMeter(32), PixelToMeter(16) };
  m.collideable = c;

  Soul s{};
  s.hp = 15;
  s.defense = 1;

#if !defined(DIB_IS_SERVER)
  auto texture = std::make_shared<graphics::Texture>("Slime");
  texture->Load(Path{ "./res/entity/slime.tga" });
  RenderComponent rc{ texture };
#else
  RenderComponent rc{};
#endif

  NpcSlime* npc =
    new NpcSlime{ em, id, type, std::move(m), std::move(s), std::move(rc) };
  return npc;
}

}
