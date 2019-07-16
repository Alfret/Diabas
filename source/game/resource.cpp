#include "game/resource.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/constants.hpp"

// ========================================================================== //
// ResourcePath
// ========================================================================== //

namespace dib::game {

ResourcePath::ResourcePath(String modId, Path resourcePath, Flag flags)
  : mModID(std::move(modId))
  , mResourcePath(std::move(resourcePath))
  , mFlags(flags)
{
  mPath = Path{ "./mods" }.Join(mModID).Join(mResourcePath);
}

// -------------------------------------------------------------------------- //

ResourcePath::ResourcePath(Path resourcePath, Flag flags)
  : mModID(BUILTIN_MOD_NAME)
  , mResourcePath(std::move(resourcePath))
  , mFlags(flags)
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