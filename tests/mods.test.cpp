#include "main.test.hpp"
#include "mods/loader.hpp"

TEST_SUITE("mods")
{
  TEST_CASE("basic loading")
  {
    auto mods = dib::mods::LoadMods(alflib::Path{ alflib::String{ "mods" } });
    CHECK(mods.size() == 1);
    CHECK(mods[0].info.name == "core");
  }
}
