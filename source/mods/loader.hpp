#ifndef LOADER_HPP_
#define LOADER_HPP_

#include <tuple>
#include <vector>
#include <core/types.hpp>
#include <mods/mod_info.hpp>

namespace dib
{

namespace ModLoader{

void Test();

enum class LoadResult {
  kSuccess,
  kParseFail,
  kInternalFail
};

std::string LoadResultToString(const LoadResult load_result);

std::tuple<LoadResult, ModInfo> Load(const String& file_path);

}
}

#endif//LOADER_HPP_
