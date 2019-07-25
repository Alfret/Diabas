#include "npc_rabbit.hpp"
#include <dlog.hpp>
#include "game/npc/npc_registry.hpp"
#include "game/world.hpp"

namespace dib::game {

void
NpcRabbit::Update(World& world, f64 delta)
{
  auto& moveable =
    world.GetEntityManager().GetRegistry().get<Moveable>(entity_);
  brain.Think(moveable, static_cast<f32>(delta));
}

void
NpcRabbit::OnSpawn(World&)
{
  DLOG_VERBOSE("I spawned!");
}

void
NpcRabbit::OnDeath(World&)
{
  DLOG_VERBOSE("I died!");
}

bool
NpcRabbit::Store(const EntityManager& em, alflib::RawMemoryWriter& mw) const
{
  return Npc::Store(em, mw);
}

void
NpcRabbit::Load(EntityManager& em, alflib::RawMemoryReader& mr)
{
  Npc::Load(em, mr);
}

Npc*
RabbitFactory(EntityManager& em, NpcID id, NpcType type)
{
  Moveable m{};
  m.velocity_input = 20.0f;
  m.velocity_max = 30.0f;
  m.velocity_jump = 20.0f;
  m.position = Position(TileToMeter(10), TileToMeter(13));
  m.width = 16;
  m.height = 16;
  Collideable c{};
  CollideableRect* cr = reinterpret_cast<CollideableRect*>(&c);
  cr->type = CollisionType::kRect;
  cr->rect = CollisionRectNoOffset{ PixelToMeter(15), PixelToMeter(15) };
  m.collideable = c;

  Soul s{};
  s.hp = 15;
  s.defense = 1;

#if !defined(DIB_IS_SERVER)
  auto texture = std::make_shared<graphics::Texture>("Rabbit");
  texture->Load(Path{ "./res/entity/rabbit.tga" });
  RenderComponent rc{ texture };
#else
  RenderComponent rc{};
#endif

  NpcRabbit* npc =
    new NpcRabbit{ em, id, type, std::move(m), std::move(s), std::move(rc) };
  return npc;
}

}
