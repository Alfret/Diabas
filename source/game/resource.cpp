#include "game/resource.hpp"

// ========================================================================== //
// ResourcePath
// ========================================================================== //

namespace dib::game {

ResourcePath::ResourcePath(ResourceType type,
                           const String& modId,
                           const Path& resourcePath)
  : mType(type)
  , mModID(modId)
  , mResourcePath(resourcePath)
{
  mPath = Path{ "./mods" }.Join(mModID).Join(mResourcePath);
}

// -------------------------------------------------------------------------- //

ResourcePath::ResourcePath(ResourceType type, const Path& resourcePath)
  : mType(type)
  , mModID("builtin")
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