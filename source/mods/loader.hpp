#ifndef LOADER_HPP_
#define LOADER_HPP_

#include "mods/mod.hpp"
#include <alflib/file/file.hpp>
#include <core/types.hpp>
#include <dlog.hpp>
#include <tuple>
#include <vector>
#include "script/env.hpp"

namespace dib::mods {

/** Class that loads all mods and makes them available for lookup **/
class ModLoader
{
private:
  /** Path to mods folder **/
  Path mModsFolder;
  /** Map of all loaded mods identifier by their ids **/
  std::unordered_map<String, Mod*> mMods;

public:
  /** Create mod loader with a specified path where all mods are expected to be
   * located **/
  ModLoader(const Path& modsFolder);

  /** Destruct and unload all mods **/
  ~ModLoader();

  /** Load mods from the mod directory. This function also get the script
   * environment as an argument. This is used to load the scripts into the
   * environment **/
  Result Load(script::Environment& environment);

  /** Returns whether or not the specified mod is loaded **/
  bool IsLoaded(const String& modId);

  /** Returns a mod identifier by it's id. **/
  Mod* GetModById(const String& modId);

  /** Run the update of all mods **/
  void Update(f32 delta);
};

}

#endif // LOADER_HPP_
