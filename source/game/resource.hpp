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

public:
  /** Create resource by specifying the mod it belongs to and the
   * mod-directory-relative path to the resource **/
  ResourcePath(const String& modId, const Path& resourcePath);

  /** Returns the absolute path to the resource **/
  Path GetAbsolutePath();
};

}