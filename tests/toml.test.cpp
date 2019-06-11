#include "main.test.hpp"
#include <mods/loader.hpp>

TEST_SUITE("toml") {
  TEST_CASE("basic loading") {

    const dib::String core_file_path{ "../mods/core/mod.toml" };
    auto [result, mod_info] = dib::ModLoader::Load(core_file_path);
    CHECK(result == dib::ModLoader::LoadResult::kSuccess);
  }

}
