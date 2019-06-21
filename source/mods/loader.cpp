#include "loader.hpp"
#include <cpptoml.h>
#include <dlog.hpp>

namespace dib::mods {

// ========================================================================== //
// ModLoader Implementation
// ========================================================================== //

ModLoader::ModLoader(const Path& modsFolder)
  : mModsFolder(modsFolder)
{}

// -------------------------------------------------------------------------- //

ModLoader::~ModLoader()
{
  for (auto& mod : mMods) {
    delete mod.second;
  }
}

// -------------------------------------------------------------------------- //

Result
ModLoader::Load(script::Environment& environment)
{
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
    if (result != Result::kSuccess) {
      return result;
    }
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

  return Result::kSuccess;
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

void
ModLoader::Update(f32 delta)
{
  for (auto& mod : mMods) {
    mod.second->Update(delta);
  }
}

}
