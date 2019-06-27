#include "game/mods/mod.hpp"

#include <dlog.hpp>
#include <cpptoml.h>
#include "script/util.hpp"
#include "core/assert.hpp"
#include "game/world.hpp"

namespace dib::game {

// ========================================================================== //
// Mod Implementation
// ========================================================================== //

Mod::Mod(script::Environment& scriptEnvironment)
  : mMainScript(scriptEnvironment)
{}

// -------------------------------------------------------------------------- //

Result
Mod::Load(const alflib::File& modDirectory)
{
  // Parse mod toml file
  Result result = Parse(modDirectory);
  if (result != Result::kSuccess) {
    return result;
  }

  // Setup script
  return SetupScript(modDirectory);
}

// -------------------------------------------------------------------------- //

void
Mod::Init(World& world)
{
  mMainScript.Init(world);
}

// -------------------------------------------------------------------------- //

Result
Mod::RegisterTiles(TileManager& tileManager)
{
  return mMainScript.RegisterTiles(tileManager);
}

// -------------------------------------------------------------------------- //

void
Mod::Update(f32 delta)
{
  mMainScript.Update(delta);
}

// -------------------------------------------------------------------------- //

void
Mod::OnKeyPress(Key key)
{
  mMainScript.OnKeyPress(key);
}

// -------------------------------------------------------------------------- //

void
Mod::OnKeyRelease(Key key)
{
  mMainScript.OnKeyRelease(key);
}

// -------------------------------------------------------------------------- //

Result
Mod::Parse(const alflib::File& modDirectory)
{
  // Make sure that the 'mod.toml' file exists in the mod directory
  alflib::File tomlFile = modDirectory.Open("mod.toml");
  if (!tomlFile.Exists()) {
    return Result::kMissingModToml;
  }

  DLOG_VERBOSE("Parsing mod config [{}]", tomlFile.GetPath().GetPathString());

  try {
    // Parse the TOML file
    std::shared_ptr<cpptoml::table> toml;
    try {
      toml = cpptoml::parse_file(tomlFile.GetPath().GetPathString().GetUTF8());
    } catch (const cpptoml::parse_exception& e) {
      DLOG_WARNING("failed to parse [{}] with error [{}]",
                   tomlFile.GetPath().GetPathString(),
                   e.what());
      return Result::kParseFail;
    }

    // fill out mod info
    auto info = toml->get_table("info");

    // Parse name
    auto maybe_name = info->get_as<std::string>("name");
    if (!maybe_name) {
      DLOG_WARNING("failed to parse name in [{}]",
                   tomlFile.GetPath().GetPathString());
      return Result::kParseFail;
    }
    mName = maybe_name.value_or("");

    // Parse identifier
    auto maybe_id = info->get_as<std::string>("id");
    if (!maybe_id) {
      DLOG_WARNING("failed to parse id in [{}]",
                   tomlFile.GetPath().GetPathString());
      return Result::kParseFail;
    }
    mId = maybe_id.value_or("");

    // Parse authors
    auto maybe_authors = info->get_array_of<std::string>("authors");
    if (!maybe_authors) {
      DLOG_WARNING("failed to parse authors in [{}]",
                   tomlFile.GetPath().GetPathString());
      return Result::kParseFail;
    }
    for (auto& author : *maybe_authors) {
      mAuthors.emplace_back(author);
    }

    // Parse version
    auto maybe_version_array = info->get_array_of<s64>("version");
    if (!maybe_version_array || maybe_version_array->size() != 3) {
      DLOG_WARNING("failed to parse version in [{}]",
                   tomlFile.GetPath().GetPathString());
      return Result::kParseFail;
    }
    mVersion.major = (*maybe_version_array)[0];
    mVersion.minor = (*maybe_version_array)[1];
    mVersion.patch = (*maybe_version_array)[2];

    // TODO dependencies
    // auto maybe_dependencies =
    //     info->get_array_of<cpptoml::array>("dependencies");
    // if (!maybe_depencendies) {
    //   DLOG_WARNING("failed to parse dependencies in [{}]", file_path);
    //   result = ParseResult::kParseFail;
    //   goto load_return;
    // }

  } catch (const std::exception& e) {
    return Result::kParseFail;
  }

  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

Result
Mod::SetupScript(const alflib::File& modDirectory)
{
  // Check that the file 'main.js' exists
  alflib::File mainSourceFile = modDirectory.Open("main.js");
  if (!mainSourceFile.Exists()) {
    return Result::kMissingMainSourceFile;
  }

  // Load script
  Result result =
    mMainScript.Load(mainSourceFile.GetPath().GetAbsolutePath(), mName);
  if (result != Result::kSuccess) {
    return Result::kInternalFail;
  }

  // Set script properties related to loaded mod
  script::SetProperty(
    mMainScript.GetInstance(), "name", script::CreateString(mName));
  script::SetProperty(
    mMainScript.GetInstance(), "id", script::CreateString(mId));

  return Result::kSuccess;
}

}