#include "script/expose/expose_network.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog.hpp>

#include "core/assert.hpp"
#include "script/util.hpp"
#include "network/packet.hpp"

// ========================================================================== //
// Packet Functions
// ========================================================================== //

namespace dib::script {

static JsValueRef PACKET_PROTOTYPE = JS_INVALID_REFERENCE;

// -------------------------------------------------------------------------- //

JsValueRef CHAKRA_CALLBACK
ScriptPacketWrite([[maybe_unused]] JsValueRef callee,
                  bool isConstruct,
                  JsValueRef* arguments,
                  u16 argumentCount,
                  [[maybe_unused]] void* callbackState)
{
  // Initial checks
  DIB_ASSERT(!isConstruct, "'ScriptPacketWrite' is not a constructor call");
  if (argumentCount < 2) {
    DLOG_ERROR(
      "'Packet::write()' called with too few arguments. At least 1 required");
    return JS_INVALID_REFERENCE;
  }

  // Retrieve packet
  SimplePacket* _packet = GetExternalData<SimplePacket>(arguments[0]);
  if (!_packet->isWrite) {
    DLOG_ERROR("Cannot write to a packet in "
               "'Mod::onPacketReceived()'");
    return JS_INVALID_REFERENCE;
  }
  WritePacket* packet = static_cast<WritePacket*>(_packet);

  // Write arguments depending on type
  for (u16 i = 1; i < argumentCount; i++) {
    JsValueRef arg = arguments[i];
    JsValueType type = GetValueType(arg);

    switch (type) {
      case JsNumber: {
        f64 number = GetF64(arg);
        packet->writer.Write(static_cast<u16>(PacketObjectType::kNumber));
        packet->writer.Write(number);
        break;
      }
      case JsString: {
        String str = GetString(arg);
        packet->writer.Write(static_cast<u16>(PacketObjectType::kString));
        packet->writer.Write(str);
        break;
      }
      case JsBoolean: {
        break;
      }
      case JsArray: {
        break;
      }
      case JsUndefined:
      case JsNull:
      case JsObject:
      case JsFunction:
      case JsError:
      case JsSymbol:
      case JsArrayBuffer:
      case JsTypedArray:
      case JsDataView:
      default: {
        DLOG_WARNING("Only object of type 'Number', 'String', 'Boolean' and "
                     "'Array' can be written to packets");
        return CreateValue(false);
      }
    }
  }

  return CreateValue(true);
}

// -------------------------------------------------------------------------- //

JsValueRef CHAKRA_CALLBACK
ScriptPacketRead([[maybe_unused]] JsValueRef callee,
                 bool isConstruct,
                 JsValueRef* arguments,
                 u16 argumentCount,
                 [[maybe_unused]] void* callbackState)
{
  // Initial checks
  DIB_ASSERT(!isConstruct, "'ScriptPacketWrite' is not a constructor call");
  if (argumentCount != 1) {
    DLOG_ERROR(
      "'Packet::write()' called with too many arguments. Zero (0) expected");
    return JS_INVALID_REFERENCE;
  }

  // Retrieve packet
  auto _packet = GetExternalData<SimplePacket>(arguments[0]);
  if (_packet->isWrite) {
    DLOG_WARNING("Cannot write to a packet in 'Mod::onPacketReceived()'");
    return JS_INVALID_REFERENCE;
  }
  auto packet = static_cast<ReadPacket*>(_packet);

  // Read type
  JsValueRef readObject = JS_INVALID_REFERENCE;
  auto type = static_cast<PacketObjectType>(packet->reader.Read<u16>());
  switch (type) {
    case PacketObjectType::kNumber: {
      f64 num = packet->reader.Read<f64>();
      readObject = CreateValue(num);
      break;
    }
    case PacketObjectType::kString: {
      String str = packet->reader.Read<String>();
      readObject = CreateString(str);
      break;
    }
    case PacketObjectType::kArray: {
      break;
    }
  }

  // Return read object
  return readObject;
}

// -------------------------------------------------------------------------- //

JsValueRef CHAKRA_CALLBACK
ScriptPacketGetType([[maybe_unused]] JsValueRef callee,
                    bool isConstruct,
                    JsValueRef* arguments,
                    u16 argumentCount,
                    [[maybe_unused]] void* callbackState)
{
  DIB_ASSERT(!isConstruct, "'ScriptPacketGetType' is not a constructor call");
  if (argumentCount != 1) {
    DLOG_ERROR(
      "'Packet::getType()' called with too many arguments. Zero (0) expected");
    return JS_INVALID_REFERENCE;
  }
  return GetProperty(arguments[0], "type");
}

// -------------------------------------------------------------------------- //

JsValueRef CHAKRA_CALLBACK
ScriptPacketConstructor([[maybe_unused]] JsValueRef callee,
                        bool isConstruct,
                        JsValueRef* arguments,
                        u16 argumentCount,
                        [[maybe_unused]] void* callbackState)
{
  DIB_ASSERT(isConstruct, "'ScriptPacketConstructor' is a constructor call");

  // Check that argument count is correct
  if (argumentCount != 2) {
    DLOG_ERROR("'Packet' constructor called with invalid number of "
               "arguments: {}, expected {}",
               argumentCount,
               1);
    return JS_INVALID_REFERENCE;
  }

  // Retrieve packet name
  String packetName = GetString(arguments[1]);

  // Create packet
  WritePacket* packet = new WritePacket(packetName);
  JsValueRef object = CreateExternalObject(packet, [](void* toFree) {
    WritePacket* _packet = static_cast<WritePacket*>(toFree);
    delete _packet;
  });

  // Set type property
  SetProperty(object, "type", arguments[1]);

  // Set prototype
  JsSetPrototype(object, PACKET_PROTOTYPE);
  return object;
}

}

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::script {

void
ExposeNetwork(Environment& environment)
{
  // Retrieve global
  JsValueRef global = GetGlobal();

  // Expose 'Packet'
  JsValueRef packetConstr = CreateFunction(ScriptPacketConstructor);
  SetProperty(global, "Packet", packetConstr);
  PACKET_PROTOTYPE = CreateObject();
  SetProperty(PACKET_PROTOTYPE, "write", CreateFunction(ScriptPacketWrite));
  SetProperty(PACKET_PROTOTYPE, "read", CreateFunction(ScriptPacketRead));
  SetProperty(PACKET_PROTOTYPE, "getType", CreateFunction(ScriptPacketGetType));
  SetProperty(packetConstr, "prototype", PACKET_PROTOTYPE);
}

// -------------------------------------------------------------------------- //

JsValueRef
CreateReadPacket(const Packet& packet, const String& packetName)
{
  ReadPacket* readPacket = nullptr;
  {
    readPacket = new ReadPacket(
      packetName,
      alflib::Buffer{ packet.GetPacketSize(), packet.GetPayload() });
  }
  JsValueRef object = script::CreateExternalObject(readPacket);
  JsSetPrototype(object, PACKET_PROTOTYPE);
  SetProperty(object, "type", CreateString(packetName));
  return object;
}

}