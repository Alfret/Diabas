#include "game/item/item_stack.hpp"

// ========================================================================== //
// ItemStack Implementation
// ========================================================================== //

namespace dib::game {

ItemStack::ItemStack(ItemRegistry::ItemID itemId, u32 size)
  : mItemId(itemId)
  , mStackSize(size)
{}

// -------------------------------------------------------------------------- //

bool
ItemStack::Merge(ItemStack& other)
{
  // Only merge with equal items
  if (mItemId != other.mItemId) {
    return false;
  }

  // Don't merge with differing data
  if (mValueStore != other.mValueStore) {
    return false;
  }

  // Move items
  const Item* item = ItemRegistry::Instance().GetItem(mItemId);
  u32 emptySlots = item->GetMaxStackSize() - mStackSize;
  u32 itemToMove = alflib::Min(emptySlots, other.mStackSize);
  mStackSize += itemToMove;
  other.mStackSize -= itemToMove;
  if (other.mStackSize == 0) {
    other.mItemId = ItemRegistry::Instance().GetInvalidItemID();
  }
  return true;
}

}