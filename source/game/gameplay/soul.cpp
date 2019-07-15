#include "soul.hpp"

#include <dutil/misc.hpp>
#include <limits>
#include <dlog.hpp>

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

}
