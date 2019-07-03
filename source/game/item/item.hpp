#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/macros.hpp"
#include "core/types.hpp"
#include "game/resource.hpp"

// ========================================================================== //
//
// ========================================================================== //

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(ItemStack);

/** Class representing an item **/
class Item
{
protected:
  /** Resource path **/
  ResourcePath mResourcePath;
  /** Sub-resource count **/
  Vector2I mSubResourceCount;
  /** Translation key **/
  String mTranslationKey;

public:
  /** Construct item by specifying the path to the resource. This resource is
   * only loaded on clients and are split into the specified number of
   * sub-resources, that are then written to the item atlas. By default the
   * number of sub-resources in the X and Y axis are one (1).
   *
   * The translation key is used to lookup the translation for the item
   */
  Item(ResourcePath resourcePath,
       String translationKey,
       Vector2I subResourceCount = Vector2I(1, 1));

  /** Base destructor **/
  virtual ~Item() = default;

  /** Called when the item is activated (right-click on clients). Returning
   * true means that the item handled the activation. False will instead let the
   * game know that the item did nothing when activated
   *
   * The item-stack that the item belongs to and the entity that did the
   * activation are also given.
   */
  virtual bool OnActivated(ItemStack& itemStack /** , Entity& entity**/);

  /** Returns the index of the resource for the item in the given item stack **/
  [[nodiscard]] virtual u32 GetResourceIndex(ItemStack& itemStack);

  /** Returns the key used for translation of the item name **/
  [[nodiscard]] virtual const String& GetTranslationKey(
    ItemStack& itemStack) const;

  /** Returns the key used for translation of the tile name **/
  [[nodiscard]] virtual const String& GetTranslationKey() const;

  /** Returns the resource path of the tile **/
  [[nodiscard]] const ResourcePath& GetResourcePath() const;

  /** Returns the number of sub-resources in the X and Y axis **/
  const Vector2I& GetSubResourceCount() const;
};

}