#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"

// ========================================================================== //
// ResourcePath
// ========================================================================== //

namespace dib::game {

/** Path to a mod-specific resource **/
class ResourcePath
{
private:
  /** Mod ID **/
  String mModID;
  /** Relative resource path from mod directory **/
  Path mResourcePath;

  /** Path relative to game directory **/
  Path mPath;

public:
  /** Create resource by specifying the mod it belongs to and the
   * mod-directory-relative path to the resource **/
  ResourcePath(const String& modId, const Path& resourcePath);

  /** Create a resource by specifying the path relative to the game
   * directory **/
  explicit ResourcePath(const Path& resourcePath);

  /** Returns the path to the resource relative to the game directory **/
  const Path& GetPath() const;
};

}