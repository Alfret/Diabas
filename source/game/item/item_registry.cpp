#include "game/item/item_registry.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/assert.hpp"
#include "game/constants.hpp"
#include "item_registry.hpp"

// ========================================================================== //
// ItemRegistry Implementation
// ========================================================================== //

namespace dib::game {

ItemRegistry::ItemRegistry()
{
  mBuiltin.itemInvalid =
    new Item(ResourcePath{ Path{ "./res/items/invalid.tga" } }, "invalid");
  RegisterItem(BUILTIN_MOD_NAME, "invalid", mBuiltin.itemInvalid);

  mBuiltin.itemMissing =
    new Item(ResourcePath{ Path{ "./res/items/missing.tga" } }, "missing");
  RegisterItem(BUILTIN_MOD_NAME, "missing", mBuiltin.itemMissing);
}

// -------------------------------------------------------------------------- //

ItemRegistry::~ItemRegistry()
{
  delete mBuiltin.itemInvalid;
  delete mBuiltin.itemMissing;
}

// -------------------------------------------------------------------------- //

bool
ItemRegistry::RegisterItem(const String& modId,
                           const String& registryKey,
                           Item* item)
{
  return RegisterItem(CreateRegistryKey(modId, registryKey), item);
}

// -------------------------------------------------------------------------- //

bool
ItemRegistry::RegisterItem(const String& registryKey, Item* item)
{
  DIB_ASSERT(item != nullptr, "Cannot register null item");

  // Make sure it's not already registered
  if (mItemRegistryMap.count(registryKey) > 0) {
    return false;
  }

  // Register tile with next ID
  mItems.push_back(item);
  const ItemID id = mItems.size() - 1;

  // Setup maps
  mItemRegistryMap[registryKey] = id;
  mItemMap[item] = id;
  return true;
}

// -------------------------------------------------------------------------- //

Item*
ItemRegistry::GetItem(ItemRegistry::ItemID id) const
{
  return mItems[id];
}

// -------------------------------------------------------------------------- //

Item*
ItemRegistry::GetItem(const String& registryKey) const
{
  return mItems[GetItemID(registryKey)];
}

// -------------------------------------------------------------------------- //

ItemRegistry::ItemID
ItemRegistry::GetItemID(const String& registryKey) const
{
  return mItemRegistryMap.at(registryKey);
}

// -------------------------------------------------------------------------- //

ItemRegistry::ItemID
ItemRegistry::GetItemID(Item* item) const
{
  return mItemMap.at(item);
}

// -------------------------------------------------------------------------- //

const tsl::robin_map<String, ItemRegistry::ItemID>
ItemRegistry::GetRegistryMap() const
{
  return mItemRegistryMap;
}

// -------------------------------------------------------------------------- //

String
ItemRegistry::CreateRegistryKey(const String& modId, const String& key)
{
  return modId + "$" + key;
}

}