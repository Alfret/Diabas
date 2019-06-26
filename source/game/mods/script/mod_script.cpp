#include "game/mods/script/mod_script.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "script/expose/expose_network.hpp"
#include "script/util.hpp"
#include "game/world.hpp"
#include "game/tile/tile_manager.hpp"
#include "game/mods/script/script_tile.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::game {

/** JS: Function to register a network packet **/
JsValueRef CHAKRA_CALLBACK
ScriptRegisterNetworkPacket([[maybe_unused]] JsValueRef callee,
                            bool isConstruct,
                            [[maybe_unused]] JsValueRef* arguments,
                            u16 argumentCount,
                            [[maybe_unused]] void* callbackState)
{
  // Check initial conditions
  AlfAssert(!isConstruct, "'Mod::registerPacket()' is not a constructor call");
  if (argumentCount != 2) {
    DLOG_WARNING(
      "'Mod::registerPacket()' expects one (1) argument, however got {}",
      argumentCount - 1);
    return script::CreateValue(false);
  }

  // Retrieve a pointer to the mod
  auto script = script::GetExternalData<ModScript>(
    script::GetProperty(arguments[0], "mod_ptr"));

  // Add packet
  String packetName = script::GetString(arguments[1]);
  String modId = script::GetString(script::GetProperty(arguments[0], "id"));
  JsValueRef worldObject = script::GetProperty(arguments[0], "world");
  auto world = script::GetExternalData<World>(worldObject);
  if (!world->GetNetwork().GetPacketHandler().AddDynamicPacketType(
        modId + "$" + packetName, [packetName, script](const Packet& packet) {
          script->OnPacketReceived(packet, packetName);
        })) {
    DLOG_WARNING("'Mod::registerPacket()' failed to register packet type, "
                 "there was already a packet registered with the same name");
    return script::CreateValue(false);
  }

  // Success!
  return script::CreateValue(true);
}

// -------------------------------------------------------------------------- //

/** JS: Function to send a packet over the network. If the sender is a client
 * then the network get sent to the server. Otherwise the packet is broadcast to
 * all clients **/
JsValueRef CHAKRA_CALLBACK
ScriptSendNetworkPacket([[maybe_unused]] JsValueRef callee,
                        bool isConstruct,
                        [[maybe_unused]] JsValueRef* arguments,
                        u16 argumentCount,
                        [[maybe_unused]] void* callbackState)
{
  // Check initial conditions
  DIB_ASSERT(!isConstruct,
             "'ScriptSendNetworkPacket' is not a constructor call");
  if (argumentCount != 2) {
    DLOG_WARNING("'Mod::sendPacket()' expects one (1) argument, however got {}",
                 argumentCount - 1);
    return script::CreateValue(false);
  }

  // Retrieve 'world' and 'packet'
  auto world =
    script::GetExternalData<World>(script::GetProperty(arguments[0], "world"));
  auto _packet = script::GetExternalData<script::SimplePacket>(arguments[1]);
  if (!_packet->isWrite) {
    DLOG_WARNING("Cannot write to a packet in 'Mod::onPacketReceived()'");
    return script::CreateValue(false);
  }
  auto packet = reinterpret_cast<script::WritePacket*>(_packet);

  // Setup packet name
  String modId = script::GetString(script::GetProperty(arguments[0], "id"));
  String packetName = modId + "$" + packet->packetType;

  // Build the native packet
  Packet nativePacket{};
  if (nativePacket.GetPacketCapacity() < packet->writer.GetBuffer().GetSize()) {
    nativePacket.SetPacketCapacity(packet->writer.GetBuffer().GetSize());
  }
  if (!world->GetNetwork().GetPacketHandler().BuildPacketHeader(nativePacket,
                                                                packetName)) {
    DLOG_WARNING("Failed to build native packet header");
    return script::CreateValue(false);
  }
  if (!nativePacket.SetPayload(packet->writer.GetBuffer().GetData(),
                               packet->writer.GetBuffer().GetSize())) {
    DLOG_WARNING("Failed to set native packet payload");
    return script::CreateValue(false);
  }

  // Broadcast the native packet
  world->GetNetwork().Broadcast(nativePacket);

  // Success!
  return script::CreateValue(true);
}

// -------------------------------------------------------------------------- //

/** JS: Function to check whether the script is run on a client **/
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

/** JS: Function to check whether the script is run on a server **/
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

namespace dib::game {

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

Result
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
  JsValueRef output;
  JsValueRef method = script::GetProperty(mInstance, "init");
  JsErrorCode error = script::CallMethod(method, { mInstance }, output);
  if (script::HandleException(error)) {
    return Result::kScriptError;
  }

  mInitialized = true;
  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

Result
ModScript::RegisterTiles(TileManager& tileManager)
{
  // Create an object for the tile manager
  JsValueRef tileManagerObject = CreateTileManagerScriptObject(tileManager);

  // Call the 'onRegisterTiles' function once
  JsValueRef output;
  JsValueRef method = script::GetProperty(mInstance, "onRegisterTiles");
  JsErrorCode error =
    script::CallMethod(method, { mInstance, tileManagerObject }, output);
  if (script::HandleException(error)) {
    return Result::kScriptError;
  }
  return Result::kSuccess;
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