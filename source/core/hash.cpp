#include "hash.hpp"

namespace dib
{

u64
HashFNV1a(const u8* data, const u64 data_count)
{
  constexpr u64 kFNVOffsetBasis = 14695981039346656037ULL;
  constexpr u64 kFNVPrime = 1099511628211ULL;

  u64 hash = kFNVOffsetBasis;
  for (u64 i=0; i<data_count; i++) {
    hash ^= data[i];
    hash *= kFNVPrime;
  }
  return hash;
}

}
