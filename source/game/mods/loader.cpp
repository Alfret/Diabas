#include "loader.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <cpptoml.h>
#include <dlog.hpp>
#include <alflib/core/assert.hpp>

#include "game/mods/script/expose_mod.hpp"
#include "game/mods/script/expose_tile.hpp"

// ========================================================================== //
// ModLoader Implementation
// ========================================================================== //

namespace dib::game {

ModLoader::ModLoader(script::Environment& environment, const Path& modsFolder)
  : mModsFolder(modsFolder)
{
  // Expose game-specific things to mods
  ExposeModBase(environment);
  ExposeTile(environment);

  // Enumerate mod folders
  const alflib::File mods_folder(mModsFolder);
  const alflib::ArrayList<alflib::File> mods_folders =
    mods_folder.Enumerate(false);

  // Load each mod
  DLOG_INFO("Loading mods");
  for (u32 i = 0; i < mods_folders.GetSize(); i++) {
    const alflib::File modPath = mods_folder.Open(mods_folders[i].GetPath());
    Mod* mod = new Mod(environment);
    Result result = mod->Load(modPath.GetPath());
    AlfAssert(result == Result::kSuccess,
              "Failed to load mod {}",
              modPath.GetPath().GetPathString());
    mMods[mod->GetId()] = mod;
  }

  // Print list of loaded mods
  String mod_list{};
  bool first = true;
  for (auto& mod : mMods) {
    if (!first) {
      mod_list += ", ";
    } else {
      first = false;
    }
    mod_list += mod.second->GetName();
  }
  DLOG_INFO("Loaded [{}] mod(s) {{ {} }}", mMods.size(), mod_list);
}

// -------------------------------------------------------------------------- //

ModLoader::~ModLoader()
{
  for (auto& mod : mMods) {
    delete mod.second;
  }
}

// -------------------------------------------------------------------------- //

bool
ModLoader::IsLoaded(const String& modId)
{
  return mMods.find(modId) != mMods.end();
}

// -------------------------------------------------------------------------- //

Mod*
ModLoader::GetModById(const String& modId)
{
  auto entry = mMods.find(modId);
  if (entry != mMods.end()) {
    return entry->second;
  }
  return nullptr;
}

// -------------------------------------------------------------------------- //

Result
ModLoader::Init(World& world)
{
  for (auto& mod : mMods) {
    mod.second->Init(world);
  }
  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

void
ModLoader::Update(f32 delta)
{
  for (auto& mod : mMods) {
    mod.second->Update(delta);
  }
}

// -------------------------------------------------------------------------- //

void
ModLoader::OnKeyPress(Key key)
{
  for (auto& mod : mMods) {
    mod.second->OnKeyPress(key);
  }
}

// -------------------------------------------------------------------------- //

void
ModLoader::OnKeyRelease(Key key)
{
  for (auto& mod : mMods) {
    mod.second->OnKeyRelease(key);
  }
}

}
