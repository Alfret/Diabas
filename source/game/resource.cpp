#include "game/resource.hpp"

// ========================================================================== //
// ResourcePath
// ========================================================================== //

namespace dib::game {

ResourcePath::ResourcePath(const String& modId, const Path& resourcePath)
  : mModID(modId)
  , mResourcePath(resourcePath)
{}

// -------------------------------------------------------------------------- //

Path
ResourcePath::GetAbsolutePath()
{
  return Path{ "./mods" }
    .Join(mModID)
    .Join(mResourcePath)
    .GetAbsolutePath()
    .GetPathString();
}

}