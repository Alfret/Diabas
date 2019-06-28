#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <alflib/memory/memory_reader.hpp>
#include <alflib/memory/memory_writer.hpp>
#include "script/env.hpp"
#include "network/packet.hpp"

// ========================================================================== //
// Enumerations
// ========================================================================== //

namespace dib::script {

/** Types of objects in a packet **/
enum class PacketObjectType : u16
{
  kNumber,
  kString,
  kArray
};

}

// ========================================================================== //
// Structures
// ========================================================================== //

namespace dib::script {

/** Base simple packet **/
struct SimplePacket
{
  /** True if the packet is a write packet, otherwise false **/
  bool isWrite;
  /** Packet type **/
  String packetType;

  SimplePacket(bool isWrite, String packetType)
    : isWrite(isWrite)
    , packetType(std::move(packetType))
  {}
};

// -------------------------------------------------------------------------- //

/** Packet for writing **/
struct WritePacket : SimplePacket
{
  /** Writer for writing to the packet **/
  alflib::Buffer buffer{128};
  alflib::MemoryWriter writer{buffer};

  explicit WritePacket(String packetType)
    : SimplePacket(true, std::move(packetType))
  {}
};

// -------------------------------------------------------------------------- //

/** Packet for reading **/
struct ReadPacket : SimplePacket
{
  /** Reader **/
  alflib::MemoryReader reader;

  /** Construct read packet **/
  ReadPacket(String packetType, const alflib::Buffer& buffer)
    : SimplePacket(false, std::move(packetType))
    , reader(buffer)
  {}
};

}

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::script {

/** Expose networking to scripts **/
void
ExposeNetwork(Environment& environment);

// -------------------------------------------------------------------------- //

/** Create a read packet from a native packet **/
JsValueRef
CreateReadPacket(const Packet& packet, const String& packetName);

}
