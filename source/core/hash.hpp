#ifndef HASH_HPP_
#define HASH_HPP_

#include "core/types.hpp"
#include <string_view>

namespace dib {

/**
 * Compute the hash of a buffer, according to FNV-1a:
 * https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 */
u64
HashFNV1a64(const u8* data, const u64 data_count);

inline u64
HashFNV1a64(const std::string_view view)
{
  return HashFNV1a64(reinterpret_cast<const u8*>(view.data()), view.size());
}

inline u64
HashFNV1a64(const char8* data, const u64 data_count)
{
  return HashFNV1a64(reinterpret_cast<const u8*>(data), data_count);
}

u32
HashFNV1a32(const u8* data, const u32 data_count);

inline u32
HashFNV1a32(const std::string_view view)
{
  return HashFNV1a32(reinterpret_cast<const u8*>(view.data()), view.size());
}

inline u32
HashFNV1a32(const char8* data, const u32 data_count)
{
  return HashFNV1a32(reinterpret_cast<const u8*>(data), data_count);
}
}

#endif // HASH_HPP_
