#pragma once

// ========================================================================== //
//
// ========================================================================== //

#include "core/macros.hpp"
#include "script/util.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(World);

// -------------------------------------------------------------------------- //

/** Expose World functionality to scripts **/
void
ExposeWorld();

// -------------------------------------------------------------------------- //

/** Create a JavaScript object for a World **/
JsValueRef
CreateScriptWorldObject(World* world);

}