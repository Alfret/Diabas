#include "game/resource.hpp"

// ========================================================================== //
// ResourcePath
// ========================================================================== //

namespace dib::game {

ResourcePath::ResourcePath(const String& modId, const Path& resourcePath)
  : mModID(modId)
  , mResourcePath(resourcePath)
{
  mPath = Path{ "./mods" }.Join(mModID).Join(mResourcePath);
}

// -------------------------------------------------------------------------- //

ResourcePath::ResourcePath(const Path& resourcePath)
  : mModID("builtin")
  , mResourcePath(resourcePath)
{
  mPath = mResourcePath;
}

// -------------------------------------------------------------------------- //

const Path&
ResourcePath::GetPath() const
{
  return mPath;
}

}