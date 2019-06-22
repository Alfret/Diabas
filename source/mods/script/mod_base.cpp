#include "mods/script/mod_base.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>
#include "core/assert.hpp"
#include "script/util.hpp"
#include "app/key.hpp"

// ========================================================================== //
// Private Functions
// ========================================================================== //

namespace dib::mods {

static JsValueRef MOD_PROTOTYPE = JS_INVALID_REFERENCE;

// -------------------------------------------------------------------------- //

JsValueRef
ScriptModConstructor([[maybe_unused]] JsValueRef callee,
                     bool isConstruct,
                     [[maybe_unused]] JsValueRef* arguments,
                     u16 argumentCount,
                     [[maybe_unused]] void* callbackState)
{
  // Check initial conditions
  DIB_ASSERT(isConstruct, "'ScriptModConstructor' is a constructor call");
  if (argumentCount != 1) {
    DLOG_WARNING("'Mod::constructor()' expects zero (0) arguments, got {}",
                 argumentCount);
    return JS_INVALID_REFERENCE;
  }

  JsValueRef prototype;
  JsGetPrototype(arguments[0], &prototype);

  // Create mod
  JsValueRef object = script::CreateObject();
  JsSetPrototype(object, prototype);
  return object;
}

// -------------------------------------------------------------------------- //

JsValueRef
ScriptModInit([[maybe_unused]] JsValueRef callee,
              [[maybe_unused]] bool isConstruct,
              [[maybe_unused]] JsValueRef* arguments,
              [[maybe_unused]] u16 argumentCount,
              [[maybe_unused]] void* callbackState)
{
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

JsValueRef
ScriptModUpdate([[maybe_unused]] JsValueRef callee,
                [[maybe_unused]] bool isConstruct,
                [[maybe_unused]] JsValueRef* arguments,
                [[maybe_unused]] u16 argumentCount,
                [[maybe_unused]] void* callbackState)
{
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

JsValueRef
ScriptModOnPacketReceived([[maybe_unused]] JsValueRef callee,
                          [[maybe_unused]] bool isConstruct,
                          [[maybe_unused]] JsValueRef* arguments,
                          [[maybe_unused]] u16 argumentCount,
                          [[maybe_unused]] void* callbackState)
{
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

JsValueRef
ScriptModOnKeyPress([[maybe_unused]] JsValueRef callee,
                    [[maybe_unused]] bool isConstruct,
                    [[maybe_unused]] JsValueRef* arguments,
                    [[maybe_unused]] u16 argumentCount,
                    [[maybe_unused]] void* callbackState)
{
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

JsValueRef
ScriptModOnKeyRelease([[maybe_unused]] JsValueRef callee,
                      [[maybe_unused]] bool isConstruct,
                      [[maybe_unused]] JsValueRef* arguments,
                      [[maybe_unused]] u16 argumentCount,
                      [[maybe_unused]] void* callbackState)
{
  return JS_INVALID_REFERENCE;
}

// -------------------------------------------------------------------------- //

void
ExposeModKeyPresses()
{
  // Retrieve global
  JsValueRef global = script::GetGlobal();
  JsValueRef keys = script::CreateObject();
  script::SetProperty(global, "keys", keys);

  script::SetProperty(keys, "SPACE", script::CreateValue(u32(Key::kKeySpace)));
  script::SetProperty(
    keys, "APOSTROPHE", script::CreateValue(u32(Key::kKeyApostrophe)));
  script::SetProperty(keys, "COMMA", script::CreateValue(u32(Key::kKeyComma)));
  script::SetProperty(keys, "MINUS", script::CreateValue(u32(Key::kKeyMinus)));
  script::SetProperty(
    keys, "PERIOD", script::CreateValue(u32(Key::kKeyPeriod)));
  script::SetProperty(keys, "SLASH", script::CreateValue(u32(Key::kKeySlash)));
  script::SetProperty(keys, "NUM_0", script::CreateValue(u32(Key::kKey0)));
  script::SetProperty(keys, "NUM_1", script::CreateValue(u32(Key::kKey1)));
  script::SetProperty(keys, "NUM_2", script::CreateValue(u32(Key::kKey2)));
  script::SetProperty(keys, "NUM_3", script::CreateValue(u32(Key::kKey3)));
  script::SetProperty(keys, "NUM_4", script::CreateValue(u32(Key::kKey4)));
  script::SetProperty(keys, "NUM_5", script::CreateValue(u32(Key::kKey5)));
  script::SetProperty(keys, "NUM_6", script::CreateValue(u32(Key::kKey6)));
  script::SetProperty(keys, "NUM_7", script::CreateValue(u32(Key::kKey7)));
  script::SetProperty(keys, "NUM_8", script::CreateValue(u32(Key::kKey8)));
  script::SetProperty(keys, "NUM_9", script::CreateValue(u32(Key::kKey9)));
  script::SetProperty(
    keys, "SEMICOLON", script::CreateValue(u32(Key::kKeySemicolon)));
  script::SetProperty(keys, "EQUAL", script::CreateValue(u32(Key::kKeyEqual)));
  script::SetProperty(keys, "A", script::CreateValue(u32(Key::kKeyA)));
  script::SetProperty(keys, "B", script::CreateValue(u32(Key::kKeyB)));
  script::SetProperty(keys, "C", script::CreateValue(u32(Key::kKeyC)));
  script::SetProperty(keys, "D", script::CreateValue(u32(Key::kKeyD)));
  script::SetProperty(keys, "E", script::CreateValue(u32(Key::kKeyE)));
  script::SetProperty(keys, "F", script::CreateValue(u32(Key::kKeyF)));
  script::SetProperty(keys, "G", script::CreateValue(u32(Key::kKeyG)));
  script::SetProperty(keys, "H", script::CreateValue(u32(Key::kKeyH)));
  script::SetProperty(keys, "I", script::CreateValue(u32(Key::kKeyI)));
  script::SetProperty(keys, "J", script::CreateValue(u32(Key::kKeyJ)));
  script::SetProperty(keys, "K", script::CreateValue(u32(Key::kKeyK)));
  script::SetProperty(keys, "L", script::CreateValue(u32(Key::kKeyL)));
  script::SetProperty(keys, "M", script::CreateValue(u32(Key::kKeyM)));
  script::SetProperty(keys, "N", script::CreateValue(u32(Key::kKeyN)));
  script::SetProperty(keys, "O", script::CreateValue(u32(Key::kKeyO)));
  script::SetProperty(keys, "P", script::CreateValue(u32(Key::kKeyP)));
  script::SetProperty(keys, "Q", script::CreateValue(u32(Key::kKeyQ)));
  script::SetProperty(keys, "R", script::CreateValue(u32(Key::kKeyR)));
  script::SetProperty(keys, "S", script::CreateValue(u32(Key::kKeyS)));
  script::SetProperty(keys, "T", script::CreateValue(u32(Key::kKeyT)));
  script::SetProperty(keys, "U", script::CreateValue(u32(Key::kKeyU)));
  script::SetProperty(keys, "V", script::CreateValue(u32(Key::kKeyV)));
  script::SetProperty(keys, "W", script::CreateValue(u32(Key::kKeyW)));
  script::SetProperty(keys, "X", script::CreateValue(u32(Key::kKeyX)));
  script::SetProperty(keys, "Y", script::CreateValue(u32(Key::kKeyY)));
  script::SetProperty(keys, "Z", script::CreateValue(u32(Key::kKeyZ)));
  script::SetProperty(
    keys, "LEFT_BRACKET", script::CreateValue(u32(Key::kKeyLeftBracket)));
  script::SetProperty(
    keys, "BACKSLASH", script::CreateValue(u32(Key::kKeyBackslash)));
  script::SetProperty(
    keys, "RIGHT_BRACKET", script::CreateValue(u32(Key::kKeyRightBracket)));
  script::SetProperty(
    keys, "GRAVE_ACCENT", script::CreateValue(u32(Key::kKeyGraveAccent)));
  script::SetProperty(
    keys, "WORLD_1", script::CreateValue(u32(Key::kKeyWorld1)));
  script::SetProperty(
    keys, "WORLD_2", script::CreateValue(u32(Key::kKeyWorld2)));
  script::SetProperty(
    keys, "ESCAPE", script::CreateValue(u32(Key::kKeyEscape)));
  script::SetProperty(keys, "ENTER", script::CreateValue(u32(Key::kKeyEnter)));
  script::SetProperty(keys, "TAB", script::CreateValue(u32(Key::kKeyTab)));
  script::SetProperty(
    keys, "BACKSPACE", script::CreateValue(u32(Key::kKeyBackspace)));
  script::SetProperty(
    keys, "INSERT", script::CreateValue(u32(Key::kKeyInsert)));
  script::SetProperty(
    keys, "DELETE", script::CreateValue(u32(Key::kKeyDelete)));
  script::SetProperty(keys, "RIGHT", script::CreateValue(u32(Key::kKeyRight)));
  script::SetProperty(keys, "LEFT", script::CreateValue(u32(Key::kKeyLeft)));
  script::SetProperty(keys, "DOWN", script::CreateValue(u32(Key::kKeyDown)));
  script::SetProperty(keys, "UP", script::CreateValue(u32(Key::kKeyUp)));
  script::SetProperty(
    keys, "PAGE_UP", script::CreateValue(u32(Key::kKeyPageUp)));
  script::SetProperty(
    keys, "PAGE_DOWN", script::CreateValue(u32(Key::kKeyPageDown)));
  script::SetProperty(keys, "HOME", script::CreateValue(u32(Key::kKeyHome)));
  script::SetProperty(keys, "END", script::CreateValue(u32(Key::kKeyEnd)));
  script::SetProperty(
    keys, "CAPS_LOCK", script::CreateValue(u32(Key::kKeyCapsLock)));
  script::SetProperty(
    keys, "SCROLL_LOCK", script::CreateValue(u32(Key::kKeyScrollLock)));
  script::SetProperty(
    keys, "NUM_LOCK", script::CreateValue(u32(Key::kKeyNumLock)));
  script::SetProperty(
    keys, "PRINT_SCREEN", script::CreateValue(u32(Key::kKeyPrintScreen)));
  script::SetProperty(keys, "PAUSE", script::CreateValue(u32(Key::kKeyPause)));
  script::SetProperty(keys, "F1", script::CreateValue(u32(Key::kKeyF1)));
  script::SetProperty(keys, "F2", script::CreateValue(u32(Key::kKeyF2)));
  script::SetProperty(keys, "F3", script::CreateValue(u32(Key::kKeyF3)));
  script::SetProperty(keys, "F4", script::CreateValue(u32(Key::kKeyF4)));
  script::SetProperty(keys, "F5", script::CreateValue(u32(Key::kKeyF5)));
  script::SetProperty(keys, "F6", script::CreateValue(u32(Key::kKeyF6)));
  script::SetProperty(keys, "F7", script::CreateValue(u32(Key::kKeyF7)));
  script::SetProperty(keys, "F8", script::CreateValue(u32(Key::kKeyF8)));
  script::SetProperty(keys, "F9", script::CreateValue(u32(Key::kKeyF9)));
  script::SetProperty(keys, "F10", script::CreateValue(u32(Key::kKeyF10)));
  script::SetProperty(keys, "F11", script::CreateValue(u32(Key::kKeyF11)));
  script::SetProperty(keys, "F12", script::CreateValue(u32(Key::kKeyF12)));
  script::SetProperty(keys, "F13", script::CreateValue(u32(Key::kKeyF13)));
  script::SetProperty(keys, "F14", script::CreateValue(u32(Key::kKeyF14)));
  script::SetProperty(keys, "F15", script::CreateValue(u32(Key::kKeyF15)));
  script::SetProperty(keys, "F16", script::CreateValue(u32(Key::kKeyF16)));
  script::SetProperty(keys, "F17", script::CreateValue(u32(Key::kKeyF17)));
  script::SetProperty(keys, "F18", script::CreateValue(u32(Key::kKeyF18)));
  script::SetProperty(keys, "F19", script::CreateValue(u32(Key::kKeyF19)));
  script::SetProperty(keys, "F20", script::CreateValue(u32(Key::kKeyF20)));
  script::SetProperty(keys, "F21", script::CreateValue(u32(Key::kKeyF21)));
  script::SetProperty(keys, "F22", script::CreateValue(u32(Key::kKeyF22)));
  script::SetProperty(keys, "F23", script::CreateValue(u32(Key::kKeyF23)));
  script::SetProperty(keys, "F24", script::CreateValue(u32(Key::kKeyF24)));
  script::SetProperty(keys, "F25", script::CreateValue(u32(Key::kKeyF25)));
  script::SetProperty(keys, "KP_0", script::CreateValue(u32(Key::kKeyKeypad0)));
  script::SetProperty(keys, "KP_1", script::CreateValue(u32(Key::kKeyKeypad1)));
  script::SetProperty(keys, "KP_2", script::CreateValue(u32(Key::kKeyKeypad2)));
  script::SetProperty(keys, "KP_3", script::CreateValue(u32(Key::kKeyKeypad3)));
  script::SetProperty(keys, "KP_4", script::CreateValue(u32(Key::kKeyKeypad4)));
  script::SetProperty(keys, "KP_5", script::CreateValue(u32(Key::kKeyKeypad5)));
  script::SetProperty(keys, "KP_6", script::CreateValue(u32(Key::kKeyKeypad6)));
  script::SetProperty(keys, "KP_7", script::CreateValue(u32(Key::kKeyKeypad7)));
  script::SetProperty(keys, "KP_8", script::CreateValue(u32(Key::kKeyKeypad8)));
  script::SetProperty(keys, "KP_9", script::CreateValue(u32(Key::kKeyKeypad9)));
  script::SetProperty(
    keys, "KP_DECIMAL", script::CreateValue(u32(Key::kKeyKeypadDecimal)));
  script::SetProperty(
    keys, "KP_DIVIDE", script::CreateValue(u32(Key::kKeyKeypadDivide)));
  script::SetProperty(
    keys, "KP_MULTIPLY", script::CreateValue(u32(Key::kKeyKeypadMultiply)));
  script::SetProperty(
    keys, "KP_SUBTRACT", script::CreateValue(u32(Key::kKeyKeypadSubtract)));
  script::SetProperty(
    keys, "KP_ADD", script::CreateValue(u32(Key::kKeyKeypadAdd)));
  script::SetProperty(
    keys, "KP_ENTER", script::CreateValue(u32(Key::kKeyKeypadEnter)));
  script::SetProperty(
    keys, "KP_EQUAL", script::CreateValue(u32(Key::kKeyKeypadEqual)));
  script::SetProperty(
    keys, "SHIFT_LEFT", script::CreateValue(u32(Key::kKeyShiftLeft)));
  script::SetProperty(
    keys, "CTRL_LEFT", script::CreateValue(u32(Key::kKeyCtrlLeft)));
  script::SetProperty(
    keys, "ALT_LEFT", script::CreateValue(u32(Key::kKeyAltLeft)));
  script::SetProperty(
    keys, "SUPER_LEFT", script::CreateValue(u32(Key::kKeySuperLeft)));
  script::SetProperty(
    keys, "SHIFT_RIGHT", script::CreateValue(u32(Key::kKeyShiftRight)));
  script::SetProperty(
    keys, "CTRL_RIGHT", script::CreateValue(u32(Key::kKeyCtrlRight)));
  script::SetProperty(
    keys, "ALT_RIGHT", script::CreateValue(u32(Key::kKeyAltRight)));
  script::SetProperty(
    keys, "SUPER_RIGHT", script::CreateValue(u32(Key::kKeySuperRight)));
  script::SetProperty(keys, "MENU", script::CreateValue(u32(Key::kKeyMenu)));
}

}

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::mods {

void
ExposeModBase(script::Environment& environment)
{
  // Retrieve global
  JsValueRef global = script::GetGlobal();

  // Setup 'Mod' constructor
  JsValueRef modConstr = script::CreateFunction(ScriptModConstructor);
  script::SetProperty(global, "Mod", modConstr);

  // Setup 'Mod' methods
  MOD_PROTOTYPE = script::CreateObject();
  script::SetProperty(
    MOD_PROTOTYPE, "init", script::CreateFunction(ScriptModInit));
  script::SetProperty(
    MOD_PROTOTYPE, "update", script::CreateFunction(ScriptModUpdate));
  script::SetProperty(MOD_PROTOTYPE,
                      "onPacketReceived",
                      script::CreateFunction(ScriptModOnPacketReceived));
  script::SetProperty(
    MOD_PROTOTYPE, "onKeyPress", script::CreateFunction(ScriptModOnKeyPress));
  script::SetProperty(MOD_PROTOTYPE,
                      "onKeyRelease",
                      script::CreateFunction(ScriptModOnKeyRelease));
  script::SetProperty(modConstr, "prototype", MOD_PROTOTYPE);

  // Setup constants
  ExposeModKeyPresses();
}

}