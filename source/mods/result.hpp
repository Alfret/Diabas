#pragma once

#include "core/types.hpp"

namespace dib::mods {

// ========================================================================== //
// Enumerations
// ========================================================================== //

/** Mod results **/
enum class Result
{
  /** Success **/
  kSuccess,
  /** Missing 'mod.toml' file **/
  kMissingModToml,
  /** Missing main mod source file 'main.js' **/
  kMissingMainSourceFile,
  /** Failed to parse 'mod.toml' file **/
  kParseFail,
  /** Internal failure **/
  kInternalFail,
  /** Missing the main mod class **/
  kMissingModClass
};

// ========================================================================== //
// Functions
// ========================================================================== //

/** Convert 'Result' to 'String' **/
String
ResultToString(Result result);

}