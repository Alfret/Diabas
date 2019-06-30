#include "game/client/debug_ui.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "app/client/imgui/imgui.h"
#include "game/client/game_client.hpp"
#include "game/client/player_data_storage.hpp"
#include "game/tile/tile_registry.hpp"
#include "script/util.hpp"

// ========================================================================== //
// DebugUI Implementation
// ========================================================================== //

namespace dib::game {

void
ShowStatisticsDebug(GameClient& gameClient, f32 delta)
{
  if (ImGui::CollapsingHeader("Statistics")) {
    ImGui::BulletText("Frame time: %.4f", delta);
    ImGui::BulletText("Frames per second: %i", u32(1.0f / delta));
    ImGui::BulletText("Draw calls: %i",
                      gameClient.GetRenderer().GetDrawCallCount());
    ImGui::BulletText("Sprites drawn: %i",
                      gameClient.GetRenderer().GetDrawSpriteCount());
  }
}

// -------------------------------------------------------------------------- //

void
ShowScriptDebug(GameClient& gameClient)
{
  if (ImGui::CollapsingHeader("Script")) {
    static constexpr u32 bufferSize = 4096;
    static char inputBuffer[bufferSize];
    static char outputBuffer[bufferSize];
    if (ImGui::InputText("Script input",
                         inputBuffer,
                         bufferSize,
                         ImGuiInputTextFlags_EnterReturnsTrue)) {
      JsValueRef output;
      script::Result result =
        gameClient.GetScriptEnvironment().RunScript(inputBuffer, output);
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
  }
}

// -------------------------------------------------------------------------- //

void
ShowModDebug(GameClient& gameClient)
{
  if (ImGui::CollapsingHeader("Mods")) {
    static s32 currentIndex = 0;
    std::vector<const char8*> names;
    for (auto& mod : gameClient.GetModLoader().GetMods()) {
      names.push_back(mod.first.GetUTF8());
    }
    ImGui::ListBox("", &currentIndex, names.data(), names.size());
    auto mod = gameClient.GetModLoader().GetModById(names[currentIndex]);
    ImGui::Text("Name: '%s'", mod->GetName().GetUTF8());
    ImGui::Text("Id: '%s'", mod->GetId().GetUTF8());
    ImGui::Text("Authors:");
    for (auto& author : mod->GetAuthors()) {
      ImGui::Text("  %s", author.GetUTF8());
    }
  }
}

// -------------------------------------------------------------------------- //

void
ShowTileDebug(GameClient& gameClient)
{
  TileRegistry& tileRegistry = gameClient.GetWorld().GetTileRegistry();
  Terrain& terrain = gameClient.GetWorld().GetTerrain();

  if (ImGui::CollapsingHeader("Tile")) {
    // Display registered tiles
    if (ImGui::TreeNode("Registered Tiles")) {
      std::vector<String> tileNames;
      for (auto& entry : tileRegistry.GetRegistryMap()) {
        tileNames.push_back(entry.first);
      }
      static s32 listIndex = 0;
      const auto NameRetrieval =
        [](void* data, s32 n, const char** textOut) -> bool {
        auto _tileNames = static_cast<std::vector<String>*>(data);
        *textOut = (*_tileNames)[n].GetUTF8();
        return true;
      };
      ImGui::ListBox("",
                     &listIndex,
                     NameRetrieval,
                     static_cast<void*>(&tileNames),
                     s32(tileNames.size()));

      // List information
      TileRegistry::TileID tileId =
        tileRegistry.GetTileID(tileNames[listIndex]);
      Tile* tile = tileRegistry.GetTiles()[tileId];
      ImGui::Text("ID: %i", tileId);
      ImGui::Text("Mod ID: %s", tile->GetResourcePath().GetModId().GetUTF8());
      ImGui::Text("Resource: \"%s\"",
                  tile->GetResourcePath().GetPath().GetPathString().GetUTF8());

      // List subresources
      ImGui::Text("Sub-resource count: %zu",
                  gameClient.GetCache().GetSubResources(tileId).size());

      ImGui::TreePop();
    }

    // Display tile by ID
    if (ImGui::TreeNode("By ID")) {
      static s32 id = 0;
      ImGui::InputInt("ID", &id);
      id = alflib::Clamp(id, 0, s32(tileRegistry.GetTiles().size() - 1));
      Tile* tile = tileRegistry.GetTile(id);
      ImGui::Text("Mod ID: %s", tile->GetResourcePath().GetModId().GetUTF8());
      ImGui::Text("Resource: \"%s\"",
                  tile->GetResourcePath().GetPath().GetPathString().GetUTF8());

      ImGui::TreePop();
    }

    // Display information about tile in world
    if (ImGui::TreeNode("In world")) {
      static s32 position[2] = { 0, 0 };
      ImGui::InputInt("X", &position[0], 1, 100);
      ImGui::InputInt("Y", &position[1], 1, 100);
      position[0] = alflib::Clamp(u32(position[0]), 0u, terrain.GetWidth());
      position[1] = alflib::Clamp(u32(position[1]), 0u, terrain.GetHeight());

      // Tile* tile =

      ImGui::TreePop();
    }

    // Display tile atlas
    if (ImGui::TreeNode("Tile atlas")) {

      static s32 atlasSize[2] = { 256, 256 };
      static bool uniformAtlasSize = true;
      ImGui::Checkbox("Uniform size", &uniformAtlasSize);
      ImGui::SliderInt("Width", &atlasSize[0], 64, 2048);
      ImGui::SliderInt("Height", &atlasSize[1], 64, 2048);
      if (uniformAtlasSize) {
        atlasSize[1] = atlasSize[0];
      }

      const auto& atlasTexture = gameClient.GetCache().GetTileAtlasTexture();
      ImGui::Image(reinterpret_cast<ImTextureID>(atlasTexture->GetID()),
                   ImVec2(atlasSize[0], atlasSize[1]),
                   ImVec2(0, 0),
                   ImVec2(1, 1),
                   ImVec4(1, 1, 1, 1),
                   ImVec4(0, 0, 0, 1));

      ImGui::TreePop();
    }
  }
}

// -------------------------------------------------------------------------- //

void
ShowNetworkDebug(GameClient& gameClient)
{
  World& world = gameClient.GetWorld();
  auto& chat = world.GetChat();

  // ImGui::ShowTestWindow();

  if (ImGui::CollapsingHeader("Network")) {

    ImGui::Text("Network status: %s",
                world.GetNetwork().GetConnectionState() ==
                    ConnectionState::kConnected
                  ? "connected"
                  : "disconnected");

    if (world.GetNetwork().GetConnectionState() ==
        ConnectionState::kConnected) {
      if (ImGui::Button("disconnect")) {
        world.GetNetwork().Disconnect();
      }
    } else {
      constexpr std::size_t addrlen = 50;
      static char8 addr[addrlen] = "127.0.0.1:24812";
      ImGui::InputText("IP Adress", addr, addrlen);
      if (ImGui::Button("connect")) {
        world.GetNetwork().ConnectToServer(addr);
      }
    }

    if (ImGui::TreeNode("Chat")) {
      constexpr std::size_t buflen = 50;
      static char8 buf[buflen] = "Rully";
      ImGui::InputText("Name", buf, buflen);
      if (ImGui::Button("Set Name")) {
        if (world.GetNetwork().GetConnectionState() ==
            ConnectionState::kDisconnected) {
          auto& player_data = PlayerDataStorage::Load();
          player_data.name = buf;
        }
      }
      ImGui::Text("Note: name can only be applied when disconnected.");

      ImGui::Spacing();
      ImGui::Spacing();

      constexpr std::size_t textlen = 128;
      static char8 text[textlen] = "";
      ImGui::InputText("chat", text, textlen);
      if (ImGui::Button("send chat message")) {
        game::ChatMessage msg{};
        msg.msg = String(text);
        msg.type = game::ChatType::kSay;
        msg.uuid_from = world.GetNetwork().GetOurUuid();
        if (!chat.SendMessage(msg)) {
          DLOG_WARNING("failed to send chat message");
        }
        text[0] = '\0';
      }

      ImGui::InputTextMultiline("",
                                const_cast<char8*>(chat.GetDebug().GetUTF8()),
                                chat.GetDebug().GetSize(),
                                ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16),
                                ImGuiInputTextFlags_AllowTabInput |
                                  ImGuiInputTextFlags_ReadOnly);

      ImGui::TreePop();
    }
  }
}
}