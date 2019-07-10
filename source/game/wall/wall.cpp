#include "game/wall/wall.hpp"

// ========================================================================== //
// Wall Implementation
// ========================================================================== //

namespace dib::game {

Wall::Wall(ResourcePath resourcePath, String translationKey)
  : mResourcePath(std::move(resourcePath))
  , mTranslationKey(std::move(translationKey))
{}

// -------------------------------------------------------------------------- //

u32
Wall::GetResourceIndex([[maybe_unused]] World& world,
                       [[maybe_unused]] WorldPos pos)
{
  return 0;
}

// -------------------------------------------------------------------------- //

const String&
Wall::GetTranslationKey([[maybe_unused]] World& world,
                        [[maybe_unused]] WorldPos pos) const
{
  return mTranslationKey;
}

// -------------------------------------------------------------------------- //

const String&
Wall::GetTranslationKey() const
{
  return mTranslationKey;
}

// -------------------------------------------------------------------------- //

const ResourcePath&
Wall::GetResourcePath() const
{
  return mResourcePath;
}

}