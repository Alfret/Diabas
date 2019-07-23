#include "brain_simple.hpp"
#include "game/gameplay/moveable.hpp"

namespace dib::game {

void
BrainSimple::Think(Moveable& m, f32 delta)
{
  static dutil::RNG<s32> action_rng{ 0, 3 };
  static dutil::RNG<f32> think_rng{ 0.5f, 5.0f };

  m.input = GameInput{};

  if (think_time_ > 0.0f) {
    switch (action_) {
      case Action::kNothing:
        think_time_ -= delta;
        break;
      case Action::kLeft:
        think_time_ -= delta * 2;
        m.input.ActionLeft();
        break;
      case Action::kRight:
        think_time_ -= delta * 2;
        m.input.ActionRight();
        break;
      case Action::kJump:
        m.input.ActionJump();
        think_time_ -= delta * 5;
        break;
    }
  } else {
    think_time_ = think_rng();
    action_ = static_cast<Action>(action_rng());
  }
}

}
