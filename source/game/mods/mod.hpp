#ifndef MOD_HPP_
#define MOD_HPP_

// ========================================================================== //
// Headers
// ========================================================================== //

#include <alflib/file/file.hpp>

#include "app/key.hpp"
#include "core/types.hpp"
#include "core/macros.hpp"
#include "game/mods/result.hpp"
#include "game/mods/script/mod_script.hpp"
#include "script/env.hpp"

// ========================================================================== //
// Mod Declaration
// ========================================================================== //

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(ItemRegistry);
DIB_FORWARD_DECLARE_CLASS(TileRegistry);
DIB_FORWARD_DECLARE_CLASS(World);

/** Mod class **/
class Mod
{
public:
  // TODO(Filip Björklund): Create a semantic version class/structure
  /** Version **/
  struct ModVersion
  {
    s32 major;
    s32 minor;
    s32 patch;
  };

  /** Dependency on another mod **/
  struct ModDependency
  {
    String name;
    ModVersion version;
  };

private:
  /** Name of the mod **/
  String mName;
  /** Id of the mod **/
  String mId;
  /** List of authors **/
  std::vector<String> mAuthors;
  /** Mod version **/
  ModVersion mVersion;
  /** List of dependencies that needs to be loaded before this mod **/
  std::vector<ModDependency> mDependencies;

  /** Main script **/
  ModScript mMainScript;

public:
  /** Construct an empty mod object. A toml file must be parsed before the mod i
   * used. The environment that scripts are loaded into is also specified **/
  explicit Mod(script::Environment& scriptEnvironment);

  /** Load mod by specifying the root directory of the mod **/
  Result Load(const alflib::File& modDirectory);

  /** Init script **/
  void Init(ItemRegistry& itemRegistry,
            TileRegistry& tileRegistry,
            World& world);

  /** Update mod **/
  void Update(f32 delta);

  /** Called when a key has been pressed **/
  void OnKeyPress(Key key);

  /** Called when a key has been released **/
  void OnKeyRelease(Key key);

  /** Returns the name of the mod **/
  const String& GetName() const { return mName; }

  /** Returns the id of the mod **/
  const String& GetId() const { return mId; }

  /** Returns the list of authors **/
  const std::vector<String>& GetAuthors() const { return mAuthors; }

private:
  /** Parse 'toml' file **/
  Result Parse(const alflib::File& modDirectory);

  /** Setup the script **/
  Result SetupScript(const alflib::File& modDirectory);
};

}

#endif // MOD_HPP_
