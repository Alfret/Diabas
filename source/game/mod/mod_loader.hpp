#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <tsl/robin_map.h>

#include "core/types.hpp"
#include "game/mod/mod.hpp"

// ========================================================================== //
// ModLoader Declaration
// ========================================================================== //

namespace dib::game {

class ModLoader
{
private:
  /** Path to mods directory **/
  Path mModsPath;

  /** List of all loaded mods **/
  std::vector<Mod> mMods;
  /** Map of mods by their ID **/
  tsl::robin_map<String, u32> mModMap;

public:
  /** Construct mod loader by specifying the mod directory **/
  explicit ModLoader(const Path& directory);

  /** Destruct and unload mods **/
  ~ModLoader() = default;

  /** Returns whether or not the mod with the given id is loaded **/
  bool IsLoaded(const String& id);
};

}