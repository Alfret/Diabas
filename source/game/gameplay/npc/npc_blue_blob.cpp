#include "npc_blue_blob.hpp"
#include <dlog.hpp>
#include "game/npc/npc_registry.hpp"
#include "game/world.hpp"
#include <optional>

namespace dib::game {

void
NpcBlueBlob::Update(World& world, f64 delta)
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

  bool found_player = false;
  if (auto maybe_pos = FindClosestPlayer(moveable.position); maybe_pos) {
    found_player = true;
    brain_.Think(world, static_cast<f32>(delta), moveable, *maybe_pos);
  }
  if constexpr(kSide == Side::kServer) {
      if (!found_player) {
        simple_brain_.Think(moveable, static_cast<f32>(delta));
      }
    }
}

void
NpcBlueBlob::OnSpawn(World&)
{
  DLOG_VERBOSE("I spawned!");
}

void
NpcBlueBlob::OnDeath(World&)
{
  DLOG_VERBOSE("I died!");
}

bool
NpcBlueBlob::Store(const EntityManager& em, alflib::RawMemoryWriter& mw) const
{
  return Npc::Store(em, mw);
}

void
NpcBlueBlob::Load(EntityManager& em, alflib::RawMemoryReader& mr)
{
  Npc::Load(em, mr);
}

Npc*
BlueBlobFactory(EntityManager& em, NpcID id, NpcType type)
{
  Moveable m{};
  m.velocity_input = 10.0f;
  m.velocity_max = 10.0f;
  m.velocity_jump = 14.0f;
  m.position = Position(TileToMeter(10), TileToMeter(13));
  m.width = 32;
  m.height = 32;
  Collideable c{};
  CollideableRect* cr = reinterpret_cast<CollideableRect*>(&c);
  cr->type = CollisionType::kRect;
  cr->rect = CollisionRectNoOffset{ PixelToMeter(31), PixelToMeter(31) };
  m.collideable = c;

  Soul s{};
  s.hp = 300;
  s.defense = 10;

#if !defined(DIB_IS_SERVER)
  auto texture = std::make_shared<graphics::Texture>("Blue Blob");
  texture->Load(Path{ "./res/entity/blue_blob.tga" });
  RenderComponent rc{ texture };
#else
  RenderComponent rc{};
#endif

  NpcBlueBlob* npc =
    new NpcBlueBlob{ em, id, type, std::move(m), std::move(s), std::move(rc) };
  return npc;
}

}
