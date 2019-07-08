#include "game/mod/mod.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <alflib/file/file.hpp>
#include <dlog.hpp>
#include <cpptoml.h>
#include <mono/metadata/appdomain.h>

#include "game/mod/mono_context.hpp"

// ========================================================================== //
// Mod Implementation
// ========================================================================== //

namespace dib::game {

Mod::Mod(Path path)
  : mPath(std::move(path))
{}

// -------------------------------------------------------------------------- //

Mod::Result
Mod::Load()
{
  Result result = LoadToml();
  if (result != Result::kSuccess) {
    return result;
  }
  return LoadScript();
}

// -------------------------------------------------------------------------- //

Mod::Result
Mod::LoadToml()
{
  // Make sure the 'mod.toml' file exists
  alflib::File tomlFile = alflib::File(mPath).Open("mod.toml");
  if (!tomlFile.Exists()) {
    return Result::kTomlNotFound;
  }

  // Parse the 'mod.toml' file
  try {
    // Parse the TOML file
    std::shared_ptr<cpptoml::table> toml;
    try {
      toml = cpptoml::parse_file(tomlFile.GetPath().GetPathString().GetUTF8());
    } catch (const cpptoml::parse_exception& e) {
      DLOG_WARNING("failed to parse [{}] with error [{}]",
                   tomlFile.GetPath().GetPathString(),
                   e.what());
      return Result::kTomlFailedToParse;
    }

    // fill out mod info
    auto info = toml->get_table("info");

    // Parse name
    auto maybe_name = info->get_as<std::string>("name");
    if (!maybe_name) {
      DLOG_WARNING("failed to parse name in [{}]",
                   tomlFile.GetPath().GetPathString());
      return Result::kTomlFailedToParse;
    }
    mName = maybe_name.value_or("");

    // Parse identifier
    auto maybe_id = info->get_as<std::string>("id");
    if (!maybe_id) {
      DLOG_WARNING("failed to parse id in [{}]",
                   tomlFile.GetPath().GetPathString());
      return Result::kTomlFailedToParse;
    }
    mId = maybe_id.value_or("");

    // Parse authors
    auto maybe_authors = info->get_array_of<std::string>("authors");
    if (!maybe_authors) {
      DLOG_WARNING("failed to parse authors in [{}]",
                   tomlFile.GetPath().GetPathString());
      return Result::kTomlFailedToParse;
    }
    for (auto& author : *maybe_authors) {
      mAuthors.emplace_back(author);
    }

    // Parse version
    auto maybe_version_array = info->get_array_of<s64>("version");
    if (!maybe_version_array || maybe_version_array->size() != 3) {
      DLOG_WARNING("failed to parse version in [{}]",
                   tomlFile.GetPath().GetPathString());
      return Result::kTomlFailedToParse;
    }
    // mVersion.major = (*maybe_version_array)[0];
    // mVersion.minor = (*maybe_version_array)[1];
    // mVersion.patch = (*maybe_version_array)[2];

    // Parse assembly name
    auto assemblyName = info->get_as<std::string>("assembly");
    if (!assemblyName) {
      mAssemblyName = mId + ".dll";
    } else {
      mAssemblyName = maybe_id.value_or("");
    }

  } catch (const std::exception& e) {
    return Result::kTomlFailedToParse;
  }

  // Success
  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

Mod::Result
Mod::LoadScript()
{
  // Make sure that file exists
  alflib::File assemblyFile = alflib::File(mPath).Open(mAssemblyName);
  if (!assemblyFile.Exists()) {
    return Result::kAssemblyNotFound;
  }

  // Load assembly
  mAssembly = mono_domain_assembly_open(
    MonoContext::GetDomain(), assemblyFile.GetPath().GetPathString().GetUTF8());
  if (!mAssembly) {
    return Result::kAssemblyNotFound;
  }

  // Instantiate mod
  return InstantiateModClass();
}

// -------------------------------------------------------------------------- //

Mod::Result
Mod::InstantiateModClass()
{
  // List classes in assembly
  std::vector<MonoClass*> classes =
    MonoContext::ListClassesInAssembly(mAssembly);
  for (MonoClass* cls : classes) {
    // Retrieve mod attribute
    MonoObject* mod = MonoContext::ModAttribute(cls);
    if (mod) {
      // Retrieve 'id' and 'name' fields
      MonoClassField* fieldId = MonoContext::GetFieldModId();
      MonoClassField* fieldName = MonoContext::GetFieldModName();
      MonoObject* id =
        mono_field_get_value_object(MonoContext::GetDomain(), fieldId, mod);
      MonoObject* name =
        mono_field_get_value_object(MonoContext::GetDomain(), fieldName, mod);

      // Instantite class
      mModInstance = mono_object_new(MonoContext::GetDomain(), cls);
      mono_runtime_object_init(mModInstance);

      break;
    }
  }

  return Result::kAssemblyNotFound;
}

}