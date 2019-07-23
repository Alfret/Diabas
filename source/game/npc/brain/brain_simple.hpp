#ifndef BRAIN_SIMPLE_HPP_
#define BRAIN_SIMPLE_HPP_

#include <dutil/random.hpp>
#include "core/types.hpp"

namespace dib::game {

class Moveable;

class BrainSimple
{
public:
  void Think(Moveable& m, f32 delta);

  enum class Action : s32
  {
    kNothing = 0,
    kLeft = 1,
    kRight = 2,
    kJump = 3,
  };

private:
  f32 think_time_ = -1.0f;
  Action action_ = Action::kNothing;
};

}

#endif // BRAIN_SIMPLE_HPP_
