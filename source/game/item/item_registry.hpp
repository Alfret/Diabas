#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <tsl/robin_map.h>

#include "core/types.hpp"
#include "game/item/item.hpp"

// ========================================================================== //
// ItemRegistry Declaration
// ========================================================================== //

namespace dib::game {

/** Registry that manages the registration and identification of items **/
class ItemRegistry
{
public:
  /** Item ID type **/
  using ItemID = u16;

private:
  /** List of registered items **/
  std::vector<Item*> mItems;
  /** Map of item registry-names to item IDs **/
  tsl::robin_map<String, ItemID> mItemRegistryMap;
  /** Map of items to item IDs **/
  tsl::robin_map<Item*, ItemID> mItemMap;

  /** Builtin items **/
  struct
  {
    /** Invalid item **/
    Item* itemInvalid = nullptr;
    /** Missing item **/
    Item* itemMissing = nullptr;
  } mBuiltin;

public:
  /** Construct Item registry **/
  ItemRegistry();

  /** Destruct **/
  ~ItemRegistry();

  /** Register an item with a given mod-id and registry key **/
  bool RegisterItem(const String& modId, const String& registryKey, Item* item);

  /** Register an item with a given registry key **/
  bool RegisterItem(const String& registryKey, Item* item);

  /** Returns an item given its ID **/
  [[nodiscard]] Item* GetItem(ItemID id) const;

  /** Returns an item given its registry key **/
  [[nodiscard]] Item* GetItem(const String& registryKey) const;

  /** Returns an item ID given its registry key **/
  [[nodiscard]] ItemID GetItemID(const String& registryKey) const;

  /** Returns an item ID given the item itself **/
  [[nodiscard]] ItemID GetItemID(Item* item) const;

  /** Returns the list of registered items **/
  [[nodiscard]] const std::vector<Item*>& GetItems() const { return mItems; }

  /** Returns the item registry map **/
  [[nodiscard]] const tsl::robin_map<String, ItemID>& GetRegistryMap() const;

  /** Returns the invalid item **/
  [[nodiscard]] Item* GetInvalidItem() const { return mBuiltin.itemInvalid; }

  /** Returns the invalid item **/
  [[nodiscard]] ItemRegistry::ItemID GetInvalidItemID() const;

public:
  static ItemRegistry& Instance();
  /** Create registry key **/
  static String CreateRegistryKey(const String& modId, const String& key);
};

}