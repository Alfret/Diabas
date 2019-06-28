#include "game/client/debug_ui.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "app/client/imgui/imgui.h"
#include "game/client/game_client.hpp"
#include "script/util.hpp"

// ========================================================================== //
// DebugUI Implementation
// ========================================================================== //

namespace dib::game {

DebugUI::DebugUI(GameClient& gameClient)
  : mGameClient(gameClient)
{}

// -------------------------------------------------------------------------- //

void
DebugUI::Update([[maybe_unused]] f32 delta)
{
  // ImGui
  if (ImGui::Begin("Diabas - Debug")) {
    ShowScriptDebug();
    ShowModDebug();
    ShowTileDebug();
  }
  ImGui::End();
}

// -------------------------------------------------------------------------- //

void
DebugUI::ShowScriptDebug()
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
      script::Result result =
        mGameClient.GetScriptEnvironment().RunScript(inputBuffer, output);
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

void
DebugUI::ShowModDebug()
{
  if (ImGui::TreeNode("Mods")) {
    static s32 currentIndex = 0;
    std::vector<const char8*> names;
    for (auto& mod : mGameClient.GetModLoader().GetMods()) {
      names.push_back(mod.first.GetUTF8());
    }
    ImGui::ListBox("", &currentIndex, names.data(), names.size());
    auto mod = mGameClient.GetModLoader().GetModById(names[currentIndex]);
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

void
DebugUI::ShowTileDebug()
{
  TileManager& tileManager = mGameClient.GetWorld().GetTileManager();
  Terrain& terrain = mGameClient.GetWorld().GetTerrain();

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
      tileAtPos->GetResourcePath(mGameClient.GetWorld(), pos[0], pos[1]);
    mGameClient.GetTileAtlas().GetTextureCoordinates(res, texMin, texMax);

    ImGui::Image(reinterpret_cast<ImTextureID>(
                   mGameClient.GetTileAtlas().GetTexture()->GetID()),
                 ImVec2(64, 64),
                 ImVec2(texMin.x, texMin.y),
                 ImVec2(texMax.x, texMax.y),
                 ImVec4(1, 1, 1, 1),
                 ImVec4(0, 0, 0, 1));

    // Change block
    if (mGameClient.IsMouseDown(MouseButton::kMouseButtonLeft)) {
      f64 mouseX, mouseY;
      mGameClient.GetMousePosition(mouseX, mouseY);
      u32 tileX, tileY;
      terrain.PickTile(mGameClient.GetCamera(), mouseX, mouseY, tileX, tileY);
      terrain.SetTile(tile, tileX, tileY, game::Terrain::LAYER_FOREGROUND);
    }

    ImGui::TreePop();
  }
}

}