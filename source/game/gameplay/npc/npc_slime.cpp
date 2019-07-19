#include "npc_slime.hpp"
#include <dlog.hpp>
#include "game/npc/npc_registry.hpp"

namespace dib::game {

void
NpcSlime::Update(World&, f64 )
{
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
SlimeFactory(EntityManager& em, NpcID id)
{
  Moveable m{};
  m.velocity_input = 10.0f;
  m.velocity_max = 20.0f;
  m.velocity_jump = 20.0f;
  m.position = Position(TileToMeter(10), TileToMeter(13));
  m.width = 32;
  m.height = 48;
  Collideable c{};
  CollideableRect* cr = reinterpret_cast<CollideableRect*>(&c);
  cr->type = CollisionType::kRect;
  cr->rect = CollisionRectNoOffset{PixelToMeter(32), PixelToMeter(16)};
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

  NpcSlime* npc = new NpcSlime{em, id, std::move(m), std::move(s), std::move(rc)};
  return npc;
}

}
