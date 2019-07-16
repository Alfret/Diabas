#include "soul.hpp"

#include <dutil/misc.hpp>
#include <limits>
#include <dlog.hpp>
#include "game/world.hpp"

namespace dib::game
{

bool
Soul::ApplyDamage(s32 damage)
{
  if (damage_timeout_ < 0.0f) {
    hp_ = dutil::Clamp(hp_ + damage, 0, std::numeric_limits<s32>::max());
    damage_timeout_ = 2.0f;
    DLOG_VERBOSE("applied {} damage, current hp: {}", damage, hp_);
    return true;
  }
  return false;
}

void
Soul::DamageTimeoutUpdate(const f32 delta)
{
  if (damage_timeout_ > 0.0f) {
    damage_timeout_ -= delta;
  }
}

// ============================================================ //

void
UpdateSoul(World& world, f64 delta)
{
  auto& registry = world.GetEntityManager().GetRegistry();
  auto view = registry.view<Soul>();
  for (const auto entity : view) {
    Soul& soul = view.get(entity);
    soul.DamageTimeoutUpdate(static_cast<f32>(delta));
  }
}
}
