#ifndef MOD_INFO_HPP_
#define MOD_INFO_HPP_

#include <core/common.hpp>
#include <core/types.hpp>
#include <vector>

namespace dib::mods
{

// ============================================================ //
// Structures
// ============================================================ //

struct ModVersion
{
  s32 version_major;
  s32 version_minor;
  s32 version_patch;
};

struct ModDependency
{
  String name;
  ModVersion version;
};

struct ModInfo
{
  String name;
  std::vector<String> authors;
  ModVersion version;
  std::vector<ModDependency> dependencies;
};

enum class ParseResult
{
  kSuccess,
  kParseFail,
  kInternalFail
};

// ============================================================ //
// Functions
// ============================================================ //

std::string ParseResultToString(const ParseResult load_result);

std::tuple<ParseResult, ModInfo>
Parse(const String& file_path);
}

#endif//MOD_INFO_HPP_
