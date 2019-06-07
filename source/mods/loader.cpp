#include "loader.hpp"
#include <cpptoml.h>
#include <dlog.hpp>

namespace dib
{

namespace ModLoader {

void
Test()
{
  const String core_file_path{ "../mods/core/mod.toml" };
  auto [result, mod_info] = Load(core_file_path);
  if (result == LoadResult::kSuccess) {
    // add to some list
  }
}

std::string
LoadResultToString(const LoadResult load_result)
{
  switch (load_result) {
    case LoadResult::kSuccess:
      return "success";
    case LoadResult::kParseFail:
      return "parse fail";
    case LoadResult::kInternalFail:
    default:
      return "internal fail";
  }
}

std::tuple<LoadResult, ModInfo>
Load(const String& file_path)
{
  LoadResult result = LoadResult::kSuccess;
  ModInfo mod_info{};

  DLOG_VERBOSE("parsing [{}]", file_path.StdString());

  try {

    // Parse the TOML file
    std::shared_ptr<cpptoml::table> toml;
    try {
      toml = cpptoml::parse_file(file_path.StdString());
    } catch (const cpptoml::parse_exception& e) {
      DLOG_WARNING("failed to parse [{}] with error [{}]",
                   file_path.StdString(), e.what());
      result = LoadResult::kParseFail;
      goto load_return;
    }

    // fill out mod info
    auto info = toml->get_table("info");

    auto maybe_name = info->get_as<std::string>("name");
    if (!maybe_name) {
      DLOG_WARNING("failed to parse name in [{}]", file_path.StdString());
      result = LoadResult::kParseFail;
      goto load_return;
    }
    mod_info.name = std::move(*maybe_name);

    auto maybe_authors = info->get_array_of<std::string>("authors");
    if (!maybe_authors) {
      DLOG_WARNING("failed to parse authors in [{}]", file_path.StdString());
      result = LoadResult::kParseFail;
      goto load_return;
    }
    for (auto& author : *maybe_authors) {
      mod_info.authors.push_back(std::move(author));
    }

    auto maybe_version_array = info->get_array_of<s64>("version");
    if (!maybe_version_array || maybe_version_array->size() != 3) {
      DLOG_WARNING("failed to parse version in [{}]", file_path.StdString());
      result = LoadResult::kParseFail;
      goto load_return;
    }
    mod_info.version.version_major = (*maybe_version_array)[0];
    mod_info.version.version_minor = (*maybe_version_array)[1];
    mod_info.version.version_patch = (*maybe_version_array)[2];

    // TODO dependencies
    // auto maybe_dependencies =
    //     info->get_array_of<cpptoml::array>("dependencies");
    // if (!maybe_depencendies) {
    //   DLOG_WARNING("failed to parse dependencies in [{}]", file_path);
    //   result = LoadResult::kParseFail;
    //   goto load_return;
    // }

  } catch (const std::exception& e) {
    result = LoadResult::kParseFail;
  }

load_return:

  return std::tuple<LoadResult, ModInfo>(result, std::move(mod_info));
}

}

}
