#include "units.hpp"

#include <cmath>

namespace dib::game {
s32
JumpVelocityToTiles(const f32 jump_height)
{
  constexpr f32 A = 0.0246178f;
  constexpr f32 B = 0.0139999f;
  constexpr f32 C = -0.192316f;
  return jump_height > 6.8f
           ? static_cast<s32>(std::lroundf(A * jump_height * jump_height +
                                           B * jump_height + C))
           : 0;
}
}
