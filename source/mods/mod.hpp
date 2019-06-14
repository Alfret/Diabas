#ifndef MOD_HPP_
#define MOD_HPP_

#include "mods/mod_info.hpp"
#include <alflib/file/file.hpp>

namespace dib::mods {

struct Mod
{
  ModInfo info;
  std::vector<alflib::File> scripts{};
};

}

#endif // MOD_HPP_
