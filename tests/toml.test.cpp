#include "main.test.hpp"
#include "game/mods/loader.hpp"

TEST_SUITE("toml")
{
  // TODO (filip)
  // TEST_CASE("basic loading") {

  //   const dib::String path{ "tests/res/mods/core/mod.toml" };
  //   auto [result, mod_info] = dib::mods::Parse(path);
  //   CHECK(result == dib::mods::ParseResult::kSuccess);
  // }

  // TEST_CASE("no file") {
  //   const dib::String path{"tests/res/mods/not_a_dir/not_a_file.toml"};
  //   auto [result, mod_info] = dib::mods::Parse(path);
  //   CHECK(result != dib::mods::ParseResult::kSuccess);
  // }

  // TEST_CASE("wrong file type")
  // {
  //   const dib::String path{ "tests/res/mods/core/scripts/test.js" };
  //   auto [result, mod_info] = dib::mods::Parse(path);
  //   CHECK(result != dib::mods::ParseResult::kSuccess);
  // }
}
