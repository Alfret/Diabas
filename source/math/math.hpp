#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include "core/assert.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib {

/** Returns the minimum of two values.
 * \brief Returns min of two values.
 * \tparam T Type of values.
 * \param a First value.
 * \param b Second value.
 * \return Minimum value.
 */
template<typename T>
constexpr T
Min(T a, T b)
{
  return a < b ? a : b;
}

// -------------------------------------------------------------------------- //

/** Returns the maximum of two values.
 * \brief Returns max of two values.
 * \tparam T Type of values.
 * \param a First value.
 * \param b Second value.
 * \return Maximum value.
 */
template<typename T>
constexpr T
Max(T a, T b)
{
  return a < b ? a : b;
}

// -------------------------------------------------------------------------- //

/** Returns whether or not a value is a power of two.
 * \brief Returns whether value is power of two.
 * \param value Value to check if power of two.
 * \return True if the value is a power of two, otherwise false.
 */
constexpr bool
IsPowerOfTwo(u64 value)
{
  return value && !(value & value - 1);
}

// -------------------------------------------------------------------------- //

/** Align a value 'base' to a power of two 'alignment'.
 * \brief Align value.
 * \param base Base value to align to the specified alignment.
 * \param alignment Value to align to.
 * \return Aligned value.
 */
constexpr u64
AlignPowerOfTwo(u64 base, u32 alignment)
{
  DIB_ASSERT(IsPowerOfTwo(alignment), "Alignment must be a power of two");
  return (base + (alignment - 1)) & -alignment;
}

}