#include "game/mods/script/expose_tile.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>

#include "core/assert.hpp"
#include "script/util.hpp"
#include "game/item/item_registry.hpp"

// ========================================================================== //
// ItemRegistry
// ========================================================================== //

namespace dib::game {}

// ========================================================================== //
// Item
// ========================================================================== //

namespace dib::game {

/** 'Item' prototype **/
static JsValueRef ITEM_PROTOTYPE = JS_INVALID_REFERENCE;

// -------------------------------------------------------------------------- //

JsValueRef
ScriptItemConstructor([[maybe_unused]] JsValueRef callee,
                      bool isConstruct,
                      JsValueRef* arguments,
                      u16 argumentCount,
                      [[maybe_unused]] void* callbackState)
{
  // Check initial condition
  DIB_ASSERT(isConstruct, "'Item::constructor' is a constructor call");
  if (argumentCount != 4 && argumentCount != 5) {
    DLOG_WARNING("'Item::constructor' expects three (3) or four (4) arguments, "
                 "however got {}",
                 argumentCount - 1);
    return script::CreateUndefined();
  }

  // Retrieve arguments
  JsValueRef modObject = arguments[1];
  String modId = script::GetString(script::GetProperty(modObject, "id"));
  String path = script::GetString(arguments[2]);
  String translationKey = script::GetString(arguments[3]);
  Vector2I subResourceCount(1, 1);
  if (argumentCount == 5) {
    // TODO(Filip Björklund): Read vector
  }

  // Create item
  Item* item = new Item(
    ResourcePath{ modId, Path{ path } }, translationKey, subResourceCount);
  JsValueRef itemObject = script::CreateExternalObject(item, [](void* _object) {
    Item* _item = static_cast<Item*>(_object);
    delete _item;
  });

  // Return item
  JsSetPrototype(itemObject, script::GetPrototype(arguments[0]));
  return itemObject;
}

// -------------------------------------------------------------------------- //

void
ExposeItemClass()
{
  JsValueRef global = script::GetGlobal();
  JsValueRef constructor = script::CreateFunction(ScriptItemConstructor);
  script::SetProperty(global, "Item", constructor);
  ITEM_PROTOTYPE = script::CreateObject();
  script::SetProperty(constructor, "prototype", ITEM_PROTOTYPE);
}

}

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::game {

void
ExposeItem()
{
  ExposeItemClass();
}

}