#include "mods/script/mod_script.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <script/expose/expose_network.hpp>
#include "script/util.hpp"
#include "game/world.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::mods {

JsValueRef CHAKRA_CALLBACK
ScriptRegisterNetworkPacket(JsValueRef callee,
                            bool isConstruct,
                            [[maybe_unused]] JsValueRef* arguments,
                            u16 argumentCount,
                            [[maybe_unused]] void* callbackState)
{
  // Check initial conditions
  AlfAssert(!isConstruct, "Color constructor must be a construct call");
  if (argumentCount != 2) {
    DLOG_WARNING("'Mod::registerPacket()' expects one (1) argument, however {} "
                 "argument(s) were specified",
                 argumentCount - 1);
    return script::CreateValue(false);
  }

  // Retrieve a pointer to the mod
  ModScript* script = script::GetExternalData<ModScript>(
    script::GetProperty(arguments[0], "mod_ptr"));

  // Add packet
  String packetName = script::GetString(arguments[1]);
  String modId = script::GetString(script::GetProperty(arguments[0], "id"));
  JsValueRef worldObject = script::GetProperty(arguments[0], "world");
  World* world = script::GetExternalData<World>(worldObject);
  world->GetNetwork().GetPacketHandler().AddDynamicPacketType(
    modId + "$" + packetName, [packetName, script](const Packet& packet) {
      script->OnPacketReceived(packet, packetName);
    });

  return script::CreateValue(true);
}

// -------------------------------------------------------------------------- //

JsValueRef CHAKRA_CALLBACK
ScriptSendNetworkPacket(JsValueRef callee,
                        bool isConstruct,
                        [[maybe_unused]] JsValueRef* arguments,
                        u16 argumentCount,
                        [[maybe_unused]] void* callbackState)
{
  // Check initial conditions
  DIB_ASSERT(!isConstruct,
             "'ScriptSendNetworkPacket' is not a constructor call");
  if (argumentCount != 2) {
    DLOG_WARNING("'Mod::sendPacket' expects one (1) argument, got {}",
                 argumentCount - 1);
    return script::CreateValue(false);
  }

  // Retrieve objects
  JsValueRef _this = arguments[0];
  World* world =
    script::GetExternalData<World>(script::GetProperty(_this, "world"));
  script::SimplePacket* _packet =
    script::GetExternalData<script::SimplePacket>(arguments[1]);
  if (!_packet->isWrite) {
    DLOG_WARNING("Cannot write to a packet in 'Mod::onPacketReceived()'");
    return script::CreateValue(false);
  }
  script::WritePacket* packet = static_cast<script::WritePacket*>(_packet);

  // Send the packet
  String modId = script::GetString(script::GetProperty(_this, "id"));
  String packetName = modId + "$" + packet->packetType;
  Packet nativePacket{};
  if (nativePacket.GetPacketCapacity() < packet->writer.GetBuffer().GetSize()) {
    nativePacket.SetPacketCapacity(packet->writer.GetBuffer().GetSize());
  }
  if (!world->GetNetwork().GetPacketHandler().BuildPacketHeader(nativePacket,
                                                                packetName)) {
    DLOG_WARNING("Failed to build packet header");
  }
  nativePacket.SetPayload(packet->writer.GetBuffer().GetData(),
                          packet->writer.GetBuffer().GetSize());
  world->GetNetwork().Broadcast(nativePacket);
  return script::CreateValue(true);
}

// -------------------------------------------------------------------------- //

JsValueRef
ScriptIsClient([[maybe_unused]] JsValueRef callee,
               bool isConstruct,
               [[maybe_unused]] JsValueRef* arguments,
               u16 argumentCount,
               [[maybe_unused]] void* callbackState)
{
  // Check initial conditions
  DIB_ASSERT(!isConstruct, "'ScriptWorldGetSize' is not a constructor call");
  if (argumentCount != 1) {
    return script::CreateUndefined();
  }
  return script::CreateValue(kSide == Side::kClient);
}

// -------------------------------------------------------------------------- //

JsValueRef
ScriptIsServer([[maybe_unused]] JsValueRef callee,
               bool isConstruct,
               [[maybe_unused]] JsValueRef* arguments,
               u16 argumentCount,
               [[maybe_unused]] void* callbackState)
{
  // Check initial conditions
  DIB_ASSERT(!isConstruct, "'ScriptWorldGetSize' is not a constructor call");
  if (argumentCount != 1) {
    return script::CreateUndefined();
  }
  return script::CreateValue(kSide == Side::kServer);
}

}
// ========================================================================== //
// Script Implementation
// ========================================================================== //

namespace dib::mods {

ModScript::ModScript(script::Environment& environment)
  : mEnvironment(environment)
{}

// -------------------------------------------------------------------------- //

ModScript::~ModScript()
{
  JsRelease(mOnKeyReleaseFunction, nullptr);
  JsRelease(mOnKeyPressFunction, nullptr);
  JsRelease(mOnPacketReceivedFunction, nullptr);
  JsRelease(mUpdateFunction, nullptr);
  JsRelease(mInstance, nullptr);
}

// -------------------------------------------------------------------------- //

Result
ModScript::Load(const Path& path, const String& className)
{
  mPath = path;
  mClassName = className;

  // Load the module
  mModule = mEnvironment.LoadModule(mPath);
  if (!mModule) {
    return Result::kMissingMainSourceFile;
  }

  // Find mod class
  JsValueRef moduleNamespace = mEnvironment.GetModuleNamespace(mModule);
  JsValueRef cls = JS_INVALID_REFERENCE;
  if (script::HasProperty(moduleNamespace, "default")) {
    cls = script::GetProperty(moduleNamespace, "default");
  } else {
    cls = script::GetProperty(moduleNamespace, mClassName);
    if (cls == JS_INVALID_REFERENCE) {
      return Result::kMissingModClass;
    }
  }

  // Instantiate mod
  mInstance = mEnvironment.InstantiateObject(cls);
  if (mInstance == JS_INVALID_REFERENCE) {
    return Result::kMissingModClass;
  }
  JsAddRef(mInstance, nullptr);

  // Store object with 'this' pointer in the instance
  script::SetProperty(mInstance, "mod_ptr", script::CreateExternalObject(this));

  // Retrieve functions
  mUpdateFunction = script::GetProperty(mInstance, "update");
  JsAddRef(mUpdateFunction, nullptr);
  mOnKeyPressFunction = script::GetProperty(mInstance, "onKeyPress");
  JsAddRef(mOnKeyPressFunction, nullptr);
  mOnPacketReceivedFunction =
    script::GetProperty(mInstance, "onPacketReceived");
  JsAddRef(mOnPacketReceivedFunction, nullptr);
  mOnKeyReleaseFunction = script::GetProperty(mInstance, "onKeyRelease");
  JsAddRef(mOnKeyReleaseFunction, nullptr);

  // Success
  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

void
ModScript::Init(World& world)
{
  DIB_ASSERT(!mInitialized, "Mod scripts can only be initialized once");

  // Store the world in the script
  JsValueRef worldObject = script::CreateExternalObject(&world);
  script::SetProperty(mInstance, "world", worldObject);

  // Common functions
  script::SetProperty(
    mInstance, "isClient", script::CreateFunction(ScriptIsClient));
  script::SetProperty(
    mInstance, "isServer", script::CreateFunction(ScriptIsServer));

  // Network Functions
  script::SetProperty(mInstance,
                      "registerPacketType",
                      script::CreateFunction(ScriptRegisterNetworkPacket));
  script::SetProperty(
    mInstance, "sendPacket", script::CreateFunction(ScriptSendNetworkPacket));

  // Call the initialize function once
  JsValueRef initFunction = script::GetProperty(mInstance, "init");
  JsValueRef args[] = { mInstance };
  JsValueRef output;
  JsErrorCode error = JsCallFunction(initFunction, args, 1, &output);
  (void)output;
  if (script::HandleException(error)) {
    // TODO(Filip Björklund): Exception when running function
  }

  mInitialized = true;
}

// -------------------------------------------------------------------------- //

void
ModScript::Update(f32 delta)
{
  JsValueRef args[] = { mInstance, script::CreateValue(delta) };
  JsValueRef output;
  JsErrorCode error = JsCallFunction(mUpdateFunction, args, 2, &output);
  DIB_ASSERT(error == JsNoError, "Failed to call 'Mod::update()'");
  (void)output;
}

// -------------------------------------------------------------------------- //

void
ModScript::OnPacketReceived(const Packet& packet, const String& type)
{
  DLOG_VERBOSE("Mod received packet of type {}", type);

  JsValueRef packetObject = script::CreateReadPacket(packet, type);
  JsValueRef args[] = { mInstance, packetObject };
  JsValueRef output;
  JsErrorCode error =
    JsCallFunction(mOnPacketReceivedFunction, args, 2, &output);
  script::HandleException(error);
  DIB_ASSERT(error == JsNoError, "Failed to call 'Mod::onPacketReceived()'");
  (void)output;
}

// -------------------------------------------------------------------------- //

void
ModScript::OnKeyPress(Key key)
{
  JsValueRef args[] = { mInstance, script::CreateValue(u32(key)) };
  JsValueRef output;
  JsErrorCode error = JsCallFunction(mOnKeyPressFunction, args, 2, &output);
  DIB_ASSERT(error == JsNoError, "Failed to call 'Mod::onKeyPress()'");
  (void)output;
}

// -------------------------------------------------------------------------- //

void
ModScript::OnKeyRelease(Key key)
{
  JsValueRef args[] = { mInstance, script::CreateValue(u32(key)) };
  JsValueRef output;
  JsErrorCode error = JsCallFunction(mOnKeyReleaseFunction, args, 2, &output);
  DIB_ASSERT(error == JsNoError, "Failed to call 'Mod::onKeyRelease()'");
  (void)output;
}

}