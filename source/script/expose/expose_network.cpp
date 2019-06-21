#include "script/expose/expose_network.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "network/packet_handler.hpp"

// ========================================================================== //
// Packet Functions
// ========================================================================== //

namespace dib::script {}

// ========================================================================== //
// Common Functions
// ========================================================================== //

namespace dib::script {

JsValueRef CHAKRA_CALLBACK
ScriptColorGetRed(JsValueRef callee,
                  bool isConstruct,
                  [[maybe_unused]] JsValueRef* arguments,
                  u16 argumentCount,
                  [[maybe_unused]] void* callbackState)
{
  DIB_ASSERT(!isConstruct, "Color constructor must be a construct call");
  DIB_ASSERT(argumentCount == 1, "ScriptColorGetRed expects no arguments");
  graphics::Color* color = GetExternalData<graphics::Color>(arguments[0]);
  DIB_ASSERT(color != nullptr, "External object data is null");
  return CreateValue(color->Red());
}

}

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::script {

void
ExposeNetwork(Environment& environment)
{}

}