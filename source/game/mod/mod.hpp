#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <vector>

#include "core/types.hpp"

// ========================================================================== //
// Mod Declaration
// ========================================================================== //

namespace dib::game {

/** Class representing a single modification **/
class Mod
{
public:
  /** Results **/
  enum class Result
  {
    /** Success **/
    kSuccess = 0,
    /** Failed to find 'mod.toml' configuration file **/
    kTomlNotFound,
    /** Failed to parse the 'mod.toml' configuration file **/
    kTomlFailedToParse,
    /** Failed to find .NET assembly **/
    kAssemblyNotFound
  };

private:
  /** Path to the mod directory **/
  Path mPath;

  /** Identifier of the modification **/
  String mId;
  /** Name of the modification **/
  String mName;
  /** List of authors **/
  std::vector<String> mAuthors;

  /** Name of the script assembly**/
  String mAssemblyName;
  /** Assembly **/
  MonoAssembly* mAssembly = nullptr;
  /** Mod object **/
  // MonoObject* mModInstance;

public:
  /** Construct a mod from the path to the subdirectory of the mod **/
  explicit Mod(Path path);

  /** Load the modification **/
  Result Load();

  /** Returns the identifier of the mod **/
  [[nodiscard]] const String& GetID() const { return mId; }

  /** Returns the name of the mod **/
  [[nodiscard]] const String& GetName() const { return mName; }

private:
  /** Load the 'mod.toml' file **/
  Result LoadToml();

  /** Load the script assembly as defined in the 'mod.toml' file **/
  Result LoadScript();

  /** Instantiate the mod class **/
  Result InstantiateModClass();
};

}
