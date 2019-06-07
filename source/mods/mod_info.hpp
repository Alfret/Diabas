#ifndef MOD_INFO_HPP_
#define MOD_INFO_HPP_

#include <core/string.hpp>
#include <core/types.hpp>
#include <vector>

namespace dib
{
struct ModVersion
{
  s32 version_major;
  s32 version_minor;
  s32 version_patch;
};

struct ModDependency
{
  String name;
  ModVersion version;
};

struct ModInfo
{
  String name;
  std::vector<String> authors;
  ModVersion version;
  std::vector<ModDependency> dependencies;
};
}

#endif//MOD_INFO_HPP_
