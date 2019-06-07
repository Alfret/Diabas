#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "types.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib {

/** Returns the value of the bit at the specified index.
 * \brief Returns bit value.
 * \param index Index of bit.
 * \return Value of bit at index.
 */
constexpr u64
Bit(u8 index)
{
  return 1ull << index;
}

}