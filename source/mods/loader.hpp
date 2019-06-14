#ifndef LOADER_HPP_
#define LOADER_HPP_

#include "mods/mod.hpp"
#include "mods/mod_info.hpp"
#include <alflib/file/file.hpp>
#include <core/types.hpp>
#include <dlog.hpp>
#include <tuple>
#include <vector>

namespace dib::mods {

bool
ExcludedPath(const alflib::Path& path);

/**
 * Load all mods from the mods folder.
 */
std::vector<Mod>
LoadMods(const alflib::Path& mods_folder_path);

}

#endif // LOADER_HPP_
