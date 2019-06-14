#include "loader.hpp"
#include <cpptoml.h>
#include <dlog.hpp>

namespace dib::mods
{

std::vector<Mod> LoadMods(const alflib::Path& mods_folder_path)
{
  std::vector<Mod> mods;

  const alflib::File mods_folder(mods_folder_path);
  const alflib::ArrayList<alflib::File> mods_folders = mods_folder.Enumerate(false);

  DLOG_INFO("Loading mods.");
  for (u32 i=0; i<mods_folders.GetSize(); i++) {
    const alflib::File mod_folder = mods_folder.Open(mods_folders[i].GetPath());
    const alflib::ArrayList<alflib::File> mod_folders = mod_folder.Enumerate(false);
    for (u32 c=0; c<mod_folders.GetSize(); c++) {
      // Mod top directory must contain a .toml file that describes it.
      if (mod_folders[c].GetPath().GetExtension() != alflib::Path::Extension::kToml) {
        continue;
      }

      if (mod_folders[c].GetPath().GetName() != "mod.toml") {
        DLOG_WARNING("ignoring [{}] from [{}] since its not [mod.toml]",
                     mod_folders[c].GetPath().GetName(),
                     mods_folders[i].GetPath().GetName());
        continue;
      }

      Mod mod{};

      // parse the mod.toml file
      const alflib::File file = mod_folder.Open(mod_folders[c].GetPath());
      auto [result, mod_info] = Parse(file.GetPath().GetAbsolutePath().GetPath());
      if (result != ParseResult::kSuccess) {
        DLOG_WARNING("skipping [{}] due to parse error [{}].",
                     mod_folders[c].GetPath().GetName(),
                     ParseResultToString(result));
        continue;
      }
      mod.info = mod_info;

      // check for scripts folder
      const alflib::File scripts_folder = mod_folder.Open("scripts");
      if (!scripts_folder.Exists()) {
        DLOG_WARNING("scripts folder not found for [{}]",
                     mods_folders[i].GetPath().GetName());
        continue;
      }

      // check if scripts folder empty
      const alflib::ArrayList<alflib::File> scripts = scripts_folder.Enumerate(false);
      if (scripts.GetSize() < 1) {
        DLOG_WARNING("no script files found for [{}]",
                     mods_folders[i].GetPath().GetName());
        continue;
      }

      // store all the script files
      s32 js_files_found = 0;
      for(u32 s=0; s<scripts.GetSize(); s++) {
        if (scripts[s].GetPath().GetExtensionString() == ".js") {
          ++js_files_found;
          alflib::File script = scripts_folder.Open(scripts[s].GetPath());
          mod.scripts.push_back(std::move(script));
        }
      }

      // check if no js files found
      if (js_files_found == 0) {
        DLOG_WARNING("no .js files found for [{}]",
                     mods_folders[i].GetPath().GetName());
        continue;
      }

      // only allow one .js file
      if (mod.scripts.size() > 1) {
        DLOG_WARNING("attemted to load [{}] script files, but we only support"
                     " 1 script file atm.", mod.scripts.size());
        continue;
      }

      // finally store the mod info structure
      mods.push_back(std::move(mod));
    }
  }

  std::string modlist{};
  bool first = true;
  for (auto& mod : mods) {
    if (!first) {
      modlist += ", ";
    } else {
      first = false;
    }
    modlist += mod.info.name.GetStdString();
  }
  DLOG_INFO("Loaded [{}] mod(s). Mod list [{}]", mods.size(), modlist);

  return mods;
}
}
