#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include "core/macros.hpp"
#include "script/env.hpp"
#include "mods/result.hpp"
#include "app/key.hpp"
#include "network/packet.hpp"

// ========================================================================== //
// Forward Declarations
// ========================================================================== //

namespace dib {
DIB_FORWARD_DECLARE_CLASS(World);
}

// ========================================================================== //
// Script Declaration
// ========================================================================== //

namespace dib::mods {

/** Mod script **/
class ModScript
{
private:
  /** Environment **/
  script::Environment& mEnvironment;
  /** Script module **/
  script::Environment::Module* mModule = nullptr;

  /** Path to script module **/
  Path mPath = Path{ "" };
  /** Main mod class name **/
  String mClassName = "";

  /** Whether the script has been initialized **/
  bool mInitialized = false;

  /** Class instance **/
  JsValueRef mInstance = JS_INVALID_REFERENCE;
  /** Update function **/
  JsValueRef mUpdateFunction = JS_INVALID_REFERENCE;
  /** OnPacketReceived function **/
  JsValueRef mOnPacketReceivedFunction = JS_INVALID_REFERENCE;
  /** OnKeyPress function **/
  JsValueRef mOnKeyPressFunction = JS_INVALID_REFERENCE;
  /** OnKeyRelease function **/
  JsValueRef mOnKeyReleaseFunction = JS_INVALID_REFERENCE;

public:
  /** Create script. This does not yet load the script **/
  explicit ModScript(script::Environment& environment);

  /** Destruct, destroying the instance **/
  ~ModScript();

  /** Load the script from the specified path and with an optional class name to
   * use when looking for the exported mod class **/
  Result Load(const Path& path, const String& className = "");

  /** Initialize the script with the world as the argument. This is stored in
   * the mod class so that many of the objects accessible from the world are
   * available to scripts  **/
  Result Init(World& world);

  /** Update script **/
  void Update(f32 delta);

  /** Called when a packet has been received **/
  void OnPacketReceived(const Packet& packet, const String& type);

  /** Called when a key has been pressed **/
  void OnKeyPress(Key key);

  /** Called when a key has been released **/
  void OnKeyRelease(Key key);

  /** Returns the instance of the mod class in the script **/
  JsValueRef GetInstance() const { return mInstance; }
};

}