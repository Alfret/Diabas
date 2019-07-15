#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/value_store.hpp"
#include "game/item/item_registry.hpp"

// ========================================================================== //
// ItemStack Declaration
// ========================================================================== //

namespace dib::game {

/** Class that represents a stack of a single item. The stack has a size that
 * specifies how many items it's holding. There is also a ValueStore for each
 * stack that is used to store any type of data related to the stack. **/
class ItemStack
{
private:
  /** ID of the item making up the stack **/
  ItemRegistry::ItemID mItemId;
  /** Number of items in stack **/
  u32 mStackSize;

  /** Value-store for data in the item stack **/
  ValueStore mValueStore;

public:
  /** Construct an item stack from an id of the item that the stack holds an a
   * count **/
  ItemStack(ItemRegistry::ItemID itemId, u32 size);

  /** Merge another item-stack with this one. Any remaining items that could not
   * be merged will remain in the other item-stack, and the function returns
   * false. If all items could be merged then the item-id of the other stack is
   * set to 'invalid' (0) and the count to zero (0), as well as having the
   * function return true.
   *
   * If the item stacks have differing value-store content then they cannot be
   * merged using this function.
   */
  bool Merge(ItemStack& other);

  /** Returns the size of the stack **/
  [[nodiscard]] u32 GetSize() const { return mStackSize; }

  /** Returns the value-store of the item stack **/
  [[nodiscard]] ValueStore& GetValueStore() { return mValueStore; }

  /** Returns the value-store of the item stack **/
  [[nodiscard]] const ValueStore& GetValueStore() const { return mValueStore; }
};

}