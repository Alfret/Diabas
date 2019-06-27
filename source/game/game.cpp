#include "game.hpp"

#include <dlog.hpp>
#include "network/side.hpp"
#include "app/imgui/imgui.h"
#include "script/util.hpp"

// TEMP (for thread sleep to not overwork my linux machine)
#include <chrono>
#include <thread>

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib {

/** Show the 'Script' debug tab **/
void
ShowScriptDebug(script::Environment& environment)
{
  if (ImGui::TreeNode("Script")) {
    static constexpr u32 bufferSize = 4096;
    static char inputBuffer[bufferSize];
    static char outputBuffer[bufferSize];
    if (ImGui::InputText("Script input",
                         inputBuffer,
                         bufferSize,
                         ImGuiInputTextFlags_EnterReturnsTrue)) {
      JsValueRef output;
      script::Result result = environment.RunScript(inputBuffer, output);
      if (result == script::Result::kException) {
        String exception = script::GetAndClearException();
        if (exception.GetSize() < 512) {
          inputBuffer[0] = 0;
          memcpy(outputBuffer, exception.GetUTF8(), exception.GetSize());
          outputBuffer[exception.GetSize()] = 0;
        }
      } else {
        String outputString = script::ToString(output);
        if (outputString.GetSize() < 512) {
          inputBuffer[0] = 0;
          memcpy(outputBuffer, outputString.GetUTF8(), outputString.GetSize());
          outputBuffer[outputString.GetSize()] = 0;
        }
        ImGui::SetKeyboardFocusHere(-1);
      }
    }

    ImGui::InputTextMultiline("Script output",
                              outputBuffer,
                              bufferSize,
                              ImVec2(),
                              ImGuiInputTextFlags_ReadOnly);
    ImGui::TreePop();
  }
}

// -------------------------------------------------------------------------- //

/** Show the 'Mods' debug tab **/
void
ShowModsDebug(game::ModLoader& modLoader)
{
  if (ImGui::TreeNode("Mods")) {
    static s32 currentIndex = 0;
    std::vector<const char8*> names;
    for (auto& mod : modLoader.GetMods()) {
      names.push_back(mod.first.GetUTF8());
    }
    ImGui::ListBox("", &currentIndex, names.data(), names.size());
    auto mod = modLoader.GetModById(names[currentIndex]);
    ImGui::Text("Name: '%s'", mod->GetName().GetUTF8());
    ImGui::Text("Id: '%s'", mod->GetId().GetUTF8());
    ImGui::Text("Authors:");
    for (auto& author : mod->GetAuthors()) {
      ImGui::Text("  %s", author.GetUTF8());
    }
    ImGui::TreePop();
  }
}

// -------------------------------------------------------------------------- //

/** Show the 'Tile' debug tab **/
void
ShowTileDebug(game::TileManager& tileManager,
              game::Terrain& terrain,
              World& world)
{
  if (ImGui::TreeNode("Tile")) {
    static s32 currentIndex = 0;
    std::vector<const char8*> names;
    for (auto& tile : tileManager.GetTiles()) {
      names.push_back(tile.id.GetUTF8());
    }
    ImGui::ListBox("", &currentIndex, names.data(), names.size());

    auto tile = tileManager.GetTile(names[currentIndex]);
    ImGui::Text("Id: %i", currentIndex);
    ImGui::Text("Resources: {%u}", u32(tile->GetResourcePaths().size()));
    for (auto& resource : tile->GetResourcePaths()) {
      ImGui::Text("  %s", resource.GetPath().GetPathString().GetUTF8());
    }

    ImGui::Separator();

    static s32 pos[2] = { 0, 0 };
    ImGui::InputInt("X", &pos[0], 1, 100);
    ImGui::InputInt("Y", &pos[1], 1, 100);
    pos[0] = alflib::Clamp(u32(pos[0]), 0u, terrain.GetWidth());
    pos[1] = alflib::Clamp(u32(pos[1]), 0u, terrain.GetHeight());
    std::shared_ptr<game::Tile> tileAtPos =
      terrain.GetTile(pos[0], pos[1], game::Terrain::LAYER_FOREGROUND);

    String tileAtPosStr =
      String("Tile at pos: {}, id {}")
        .Format(tileManager.GetTileID(tileAtPos->GetID()), tileAtPos->GetID());
    ImGui::Text("%s", tileAtPosStr.GetUTF8());

    Vector2F texMin, texMax;
    const game::ResourcePath& res =
      tileAtPos->GetResourcePath(world, pos[0], pos[1]);
    tileManager.GetTextureCoordinates(res, texMin, texMax);

    ImGui::Image(
      reinterpret_cast<ImTextureID>(tileManager.GetTexture()->GetID()),
      ImVec2(64, 64),
      ImVec2(texMin.x, texMin.y),
      ImVec2(texMax.x, texMax.y),
      ImVec4(1, 1, 1, 1),
      ImVec4(0, 0, 0, 1));

    ImGui::TreePop();
  }
}

}

// ========================================================================== //
// Game Implementation
// ========================================================================== //

namespace dib {

Game::Game(const Descriptor& descriptor)
  : Application(descriptor)
  , mCamera(GetWidth(), GetHeight())
  , mModLoader(Path{ "./mods" })
  , mTerrain(mTileManager, 8400, 2400)
{
  GetGraphics().SetClearColor(graphics::Color{ 100, 149, 237 });

  DLOG_INFO("running as {}", SideToString(kSide));
  SetupInputCommands();

  // Initialize script environment and load mods
  game::Result modResult = mModLoader.Load(mScriptEnvironment);
  DIB_ASSERT(modResult == game::Result::kSuccess, "Failed to load mods");
  mModLoader.Init(world_);
  mModLoader.RegisterTiles(mTileManager);

  // Create an atlas from all loaded tiles
  mTileManager.CreateAtlas();
}

// -------------------------------------------------------------------------- //

Game::~Game()
{
  // TODO do we need to explicitly call shutdown on Network here?
  std::exit(0);
}

// -------------------------------------------------------------------------- //

void
Game::Update(f64 delta)
{
  // Exit if escape is pressed
  if (IsKeyDown(Key::kKeyEscape)) {
    Exit();
  }

  // ImGui
  if (ImGui::Begin("Diabas - Debug")) {
    ShowScriptDebug(mScriptEnvironment);
    ShowModsDebug(mModLoader);
    ShowTileDebug(mTileManager, mTerrain, world_);
  }
  ImGui::End();

  //
  input_handler_.Update();

  // Update script environment and mod loader
  mScriptEnvironment.Update();
  mModLoader.Update(delta);

  // Update world
  world_.Update();

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// -------------------------------------------------------------------------- //

void
Game::Render()
{
  // Render terrain
  mTerrain.Draw(mRenderer, mCamera);
}

// -------------------------------------------------------------------------- //

void
Game::OnKeyPressed(Key key)
{
  mModLoader.OnKeyPress(key);
}

// -------------------------------------------------------------------------- //

void
Game::OnKeyReleased(Key key)
{
  mModLoader.OnKeyRelease(key);
}

// -------------------------------------------------------------------------- //

void
Game::SetupInputCommands()
{
  input_handler_.AddCommand(
    InputCommandCategory::kInfo,
    "network",
    std::bind(&World::OnCommandNetwork, &world_, std::placeholders::_1));

  input_handler_.AddCommand(
    InputCommandCategory::kChat,
    "broadcast",
    std::bind(&World::OnCommandBroadcast, &world_, std::placeholders::_1));

  input_handler_.AddCommand(
    InputCommandCategory::kSystem, "exit", [](const std::string_view) {
      DLOG_INFO("Exiting the program");
      // TODO do we need to close the network before calling exit?
      std::exit(0);
    });

  input_handler_.AddCommand(
    InputCommandCategory::kSystem, "save", [](const std::string_view) {
      DLOG_INFO("Sorry to break it to you, but you cannot save yet..");
    });

  input_handler_.AddCommand(
    InputCommandCategory::kSystem, "filip", [](const std::string_view input) {
      DLOG_INFO("filip säger {}", input);
    });
}

}
