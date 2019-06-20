#include "mods/result.hpp"

namespace dib::mods {

// ========================================================================== //
// Functions
// ========================================================================== //

String
ResultToString(Result result)
{
  switch (result) {
    case Result::kSuccess:
      return "success";
    case Result::kMissingModToml:
      return "missing 'mod.toml' in mod directory";
    case Result::kMissingMainSourceFile:
      return "main mod file ('main.js') not found";
    case Result::kParseFail:
      return "parse failure";
    case Result::kInternalFail:
      return "internal failure";
  }
}

}