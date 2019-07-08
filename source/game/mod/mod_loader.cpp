#include "game/mod/mod_loader.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <alflib/file/file.hpp>

// ========================================================================== //
// ModLoader Implementation
// ========================================================================== //

namespace dib::game {

ModLoader::ModLoader(const Path& directory)
{
  // Enumerate mod folders
  const alflib::File root(directory);
  const alflib::ArrayList<alflib::File> rootMods = root.Enumerate(false);

  // Load each modification
  for (const auto& mod : rootMods) {
    const alflib::File modDir = root.Open(mod.GetPath());
    mMods.emplace_back(modDir.GetPath());
    if (mMods.back().Load() != Mod::Result::kSuccess) {
      mMods.pop_back();
      continue;
    }
    mModMap[mMods.back().GetID()] = mMods.size() - 1;
  }
}

// -------------------------------------------------------------------------- //

bool
ModLoader::IsLoaded(const String& id)
{
  return mModMap.count(id) > 0;
}

}