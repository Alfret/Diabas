#include "game/resource.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/constants.hpp"

// ========================================================================== //
// ResourcePath
// ========================================================================== //

namespace dib::game {

ResourcePath::ResourcePath(String modId, Path resourcePath)
  : mModID(std::move(modId))
  , mResourcePath(std::move(resourcePath))
{
  mPath = Path{ "./mods" }.Join(mModID).Join(mResourcePath);
}

// -------------------------------------------------------------------------- //

ResourcePath::ResourcePath(Path resourcePath)
  : mModID(BUILTIN_MOD_NAME)
  , mResourcePath(std::move(resourcePath))
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