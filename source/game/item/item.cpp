#include "game/item/item.hpp"

// ========================================================================== //
// Item Implementation
// ========================================================================== //

namespace dib::game {

Item::Item(ResourcePath resourcePath,
           String translationKey,
           Vector2I subResourceCount)
  : mResourcePath(std::move(resourcePath))
  , mSubResourceCount(subResourceCount)
  , mTranslationKey(std::move(translationKey))
{}

// -------------------------------------------------------------------------- //

bool
Item::OnActivated([[maybe_unused]] ItemStack& itemStack)
{
  return false;
}

// -------------------------------------------------------------------------- //

Item*
Item::SetMaxStackSize(u32 maxStackSize)
{
  mMaxStackSize = maxStackSize;
  return this;
}

// -------------------------------------------------------------------------- //

u32
Item::GetResourceIndex([[maybe_unused]] ItemStack& itemStack)
{
  return 0;
}

// -------------------------------------------------------------------------- //

const String&
Item::GetTranslationKey([[maybe_unused]] ItemStack& itemStack) const
{
  return mTranslationKey;
}

// -------------------------------------------------------------------------- //

const String&
Item::GetTranslationKey() const
{
  return mTranslationKey;
}

// -------------------------------------------------------------------------- //

const ResourcePath&
Item::GetResourcePath() const
{
  return mResourcePath;
}

// -------------------------------------------------------------------------- //

const Vector2I&
Item::GetSubResourceCount() const
{
  return mSubResourceCount;
}

}