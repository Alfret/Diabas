#ifndef LOADER_HPP_
#define LOADER_HPP_

#include "game/mods/mod.hpp"
#include <alflib/file/file.hpp>
#include <core/types.hpp>
#include <dlog.hpp>
#include <tuple>
#include <vector>
#include "script/env.hpp"
#include <unordered_map>
#include "core/macros.hpp"
#include "app/key.hpp"

// ========================================================================== //
// ModLoader Declaration
// ========================================================================== //

namespace dib {
DIB_FORWARD_DECLARE_CLASS(World);
}

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(TileManager);

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
  explicit ModLoader(const Path& modsFolder);

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

  /** Initialize scripts **/
  Result Init(World& world);

  /** Callback for when the mods must register their blocks **/
  Result RegisterTiles(TileManager& tileManager);

  /** Run the update of all mods **/
  void Update(f32 delta);

  /** Notify scripts that a key has been pressed **/
  void OnKeyPress(Key key);

  /** Notify scripts that a key has been released **/
  void OnKeyRelease(Key key);
};

}

#endif // LOADER_HPP_
