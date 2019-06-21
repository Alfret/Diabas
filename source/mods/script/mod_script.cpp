#include <script/expose/expose_network.hpp>
#include "mods/script/mod_script.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

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
  AlfAssert(!isConstruct, "Color constructor must be a construct call");
  AlfAssert(argumentCount == 2, "'registerPacket' expects one (1) argument");

  JsValueRef worldObject = script::GetProperty(arguments[0], "world");
  World* world = script::GetExternalData<World>(worldObject);

  String packetName = script::GetString(arguments[1]);
  String modId = script::GetString(script::GetProperty(arguments[0], "id"));

  world->GetNetwork().GetPacketHandler().AddDynamicPacketType(
    packetName + "$" + modId,
    [&](const Packet& packet) { DLOG_VERBOSE("Received packet in mod"); });

  DLOG_VERBOSE("Mod {}: registerPacket({})", modId, packetName);

  /*graphics::Color* color = GetExternalData<graphics::Color>(arguments[0]);
  DIB_ASSERT(color != nullptr, "External object data is null");
  return CreateValue(color->Red());*/

  return JS_INVALID_REFERENCE;
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

  // Packet packet;

  return script::CreateValue(true);
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

  // Retrieve functions
  mUpdateFunction = script::GetProperty(mInstance, "update");
  JsAddRef(mUpdateFunction, nullptr);

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

  // Expose register packet
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
  JsCallFunction(mUpdateFunction, args, 2, &output);
  (void)output;
}

}