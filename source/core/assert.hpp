#pragma once

#include <cassert>

// ========================================================================== //
// Macros
// ========================================================================== //

#define DIB_ASSERT(condition, message) \
  assert(!!(condition) && message)