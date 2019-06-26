#ifndef FIXED_TIME_UPDATE_HPP_
#define FIXED_TIME_UPDATE_HPP_

#include <dutil/stopwatch.hpp>
#include <utility>

namespace dib {

struct FixedTimeUpdateInfo
{
  dutil::Stopwatch sw;
  double timer_ms;

  FixedTimeUpdateInfo()
    : sw()
    , timer_ms(0)
  {
    sw.Start();
    timer_ms = sw.fnow_ms();
  }
};

/**
 * Run the callback at ticks_per_s times a second. It will catch up if it
 * falls behind.
 *
 * Example, class method as callback function:
 *
 *   static const auto callback_fn =
 *     std::bind(&Class::Method, &instance);
 *
 *   FixedTimeUpdate(32, callback_fn);
 *
 */
template<typename TUpdateCallback>
bool
FixedTimeUpdate(const s64 ticks_per_s, TUpdateCallback&& update_callback)
{
  static FixedTimeUpdateInfo info{};
  const s64 tick_ms = static_cast<s64>(1000) / ticks_per_s;
  if (info.sw.fnow_ms() - info.timer_ms > tick_ms) {
    info.timer_ms += tick_ms;
    std::forward<TUpdateCallback>(update_callback)();
    return true;
  }
  return false;
}
}

#endif // FIXED_TIME_UPDATE_HPP_
