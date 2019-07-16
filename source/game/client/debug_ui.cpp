#include "game/client/debug_ui.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dutil/stopwatch.hpp>

#include "core/memory.hpp"
#include <imgui/imgui.h>
#include "game/client/game_client.hpp"
#include "game/client/player_data_storage.hpp"
#include "game/client/world_renderer.hpp"
#include "game/tile/tile_registry.hpp"
#include "game/ecs/systems/generic_system.hpp"
#include "game/gameplay/moveable.hpp"

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

    static u32 maxFps = 0;
    if (u32(1.0f / delta) > maxFps) {
      maxFps = u32(1.0f / delta);
    }
    ImGui::BulletText("Max fps: %i", maxFps);
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
      maxFps = 0;
    }

    f64 vmUsage, residentSet;
    core::GetVirtualMemoryUsage(vmUsage, residentSet);
    ImGui::BulletText("Memory usage (MB): %f", residentSet / 1000.0);
  }
}

// -------------------------------------------------------------------------- //

void
ShowScriptDebug([[maybe_unused]] GameClient& gameClient)
{
  /*
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
   */
}

// -------------------------------------------------------------------------- //

void
ShowModDebug([[maybe_unused]] GameClient& gameClient)
{
  /*
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
   */
}

// -------------------------------------------------------------------------- //

void
ShowTileDebug(GameClient& gameClient)
{
  TileRegistry& tileRegistry = TileRegistry::Instance();
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
                  gameClient.GetCache().GetTileSubResources(tileId).size());

      ImGui::TreePop();
    }

    // Display tile by ID
    if (ImGui::TreeNode("By ID")) {
      static s32 indices[2] = { 0, 0 };
      ImGui::SliderInt(
        "Index", &indices[0], 0, tileRegistry.GetTiles().size() - 1);
      u32 maxSubResource =
        gameClient.GetCache().GetTileSubResourceCount(indices[0]) - 1;
      indices[1] = alflib::Clamp(indices[1], 0, s32(maxSubResource));
      ImGui::SliderInt("Sub-resource", &indices[1], 0, maxSubResource);

      Vector2F texMin, texMax;
      gameClient.GetCache().GetTextureCoordinatesForTile(
        indices[0], indices[1], texMin, texMax);
      const auto& atlasTexture = gameClient.GetCache().GetTileAtlasTexture();
      Vector2F size((texMax.x - texMin.x) * atlasTexture->GetWidth() * 10,
                    (texMax.y - texMin.y) * atlasTexture->GetHeight() * 10);
      ImGui::Image(reinterpret_cast<ImTextureID>(atlasTexture->GetID()),
                   ImVec2(size.x, size.y),
                   ImVec2(texMin.x, texMin.y),
                   ImVec2(texMax.x, texMax.y),
                   ImVec4(1, 1, 1, 1),
                   ImVec4(0, 0, 0, 1));

      // Tile placement/removal
      static s32 buttonIndex = 0;
      ImGui::RadioButton("Select", &buttonIndex, 0);
      ImGui::SameLine();
      ImGui::RadioButton("Place", &buttonIndex, 1);
      ImGui::SameLine();
      ImGui::RadioButton("Remove", &buttonIndex, 2);

      if (!ImGui::IsMouseHoveringAnyWindow() &&
          gameClient.IsMouseDown(MouseButton::kMouseButtonLeft)) {
        // Retrieve world position
        f64 mouseX, mouseY;
        gameClient.GetMousePosition(mouseX, mouseY);
        WorldPos pos = gameClient.GetWorldRenderer().PickScreenTile(
          gameClient.GetCamera(), Vector2F(mouseX, mouseY));

        // Handle different modes
        if (buttonIndex == 1) {
          gameClient.GetWorld().GetTerrain().SetTile(pos, indices[0]);
        } else if (buttonIndex == 2) {
          gameClient.GetWorld().GetTerrain().RemoveTile(
            pos, CoreContent::GetTiles().air);
        }
      }

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
ShowWallDebug(GameClient& gameClient)
{
  WallRegistry& wallRegistry = WallRegistry::Instance();
  Terrain& terrain = gameClient.GetWorld().GetTerrain();

  if (ImGui::CollapsingHeader("Wall")) {
    // Display registered tiles
    if (ImGui::TreeNode("Registered walls")) {
      std::vector<String> wallNames;
      for (auto& entry : wallRegistry.GetRegistryMap()) {
        wallNames.push_back(entry.first);
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
                     static_cast<void*>(&wallNames),
                     s32(wallNames.size()));

      // List information
      WallRegistry::WallID wallId =
        wallRegistry.GetWallID(wallNames[listIndex]);
      Wall* wall = wallRegistry.GetWalls()[wallId];
      ImGui::Text("ID: %i", wallId);
      ImGui::Text("Mod ID: %s", wall->GetResourcePath().GetModId().GetUTF8());
      ImGui::Text("Resource: \"%s\"",
                  wall->GetResourcePath().GetPath().GetPathString().GetUTF8());

      // List subresources
      ImGui::Text("Sub-resource count: %zu",
                  gameClient.GetCache().GetWallSubResources(wallId).size());

      ImGui::TreePop();
    }

    // Display tile by ID
    if (ImGui::TreeNode("By ID")) {
      static s32 indices[2] = { 0, 0 };
      ImGui::SliderInt(
        "Index", &indices[0], 0, wallRegistry.GetWalls().size() - 1);
      u32 maxSubResource =
        gameClient.GetCache().GetWallSubResourceCount(indices[0]) - 1;
      indices[1] = alflib::Clamp(indices[1], 0, s32(maxSubResource));
      ImGui::SliderInt("Sub-resource", &indices[1], 0, maxSubResource);

      Vector2F texMin, texMax;
      gameClient.GetCache().GetTextureCoordinatesForWall(
        indices[0], indices[1], texMin, texMax);
      const auto& atlasTexture = gameClient.GetCache().GetWallAtlasTexture();
      Vector2F size((texMax.x - texMin.x) * atlasTexture->GetWidth() * 10,
                    (texMax.y - texMin.y) * atlasTexture->GetHeight() * 10);
      ImGui::Image(reinterpret_cast<ImTextureID>(atlasTexture->GetID()),
                   ImVec2(size.x, size.y),
                   ImVec2(texMin.x, texMin.y),
                   ImVec2(texMax.x, texMax.y),
                   ImVec4(1, 1, 1, 1),
                   ImVec4(0, 0, 0, 1));

      // Tile placement/removal
      static s32 buttonIndex = 0;
      ImGui::RadioButton("Select", &buttonIndex, 0);
      ImGui::SameLine();
      ImGui::RadioButton("Place", &buttonIndex, 1);
      ImGui::SameLine();
      ImGui::RadioButton("Remove", &buttonIndex, 2);

      if (!ImGui::IsMouseHoveringAnyWindow() &&
          gameClient.IsMouseDown(MouseButton::kMouseButtonLeft)) {
        // Retrieve world position
        f64 mouseX, mouseY;
        gameClient.GetMousePosition(mouseX, mouseY);
        WorldPos pos = gameClient.GetWorldRenderer().PickScreenTile(
          gameClient.GetCamera(), Vector2F(mouseX, mouseY));

        // Handle different modes
        if (buttonIndex == 1) {
          gameClient.GetWorld().GetTerrain().SetWall(pos, indices[0]);
        } else if (buttonIndex == 2) {
          // gameClient.GetWorld().GetTerrain().RemoveWall(
          //  pos, CoreContent::GetTiles().air);
        }
      }

      ImGui::TreePop();
    }

    // Display information about wall in world
    if (ImGui::TreeNode("In world")) {
      static s32 position[2] = { 0, 0 };
      ImGui::InputInt("X", &position[0], 1, 100);
      ImGui::InputInt("Y", &position[1], 1, 100);
      position[0] = alflib::Clamp(u32(position[0]), 0u, terrain.GetWidth());
      position[1] = alflib::Clamp(u32(position[1]), 0u, terrain.GetHeight());

      // Tile* tile =

      ImGui::TreePop();
    }

    // Display wall atlas
    if (ImGui::TreeNode("Atlas")) {

      static s32 atlasSize[2] = { 256, 256 };
      static bool uniformAtlasSize = true;
      ImGui::Checkbox("Uniform size", &uniformAtlasSize);
      ImGui::SliderInt("Width", &atlasSize[0], 64, 2048);
      ImGui::SliderInt("Height", &atlasSize[1], 64, 2048);
      if (uniformAtlasSize) {
        atlasSize[1] = atlasSize[0];
      }

      const auto& atlasTexture = gameClient.GetCache().GetWallAtlasTexture();
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
ShowItemDebug(GameClient& gameClient)
{
  if (ImGui::CollapsingHeader("Items")) {
    // Display by id
    if (ImGui::TreeNode("By ID")) {
      static s32 indices[2] = { 0, 0 };
      ImGui::SliderInt("Index",
                       &indices[0],
                       0,
                       ItemRegistry::Instance().GetItems().size() - 1);
      u32 maxSubResource =
        gameClient.GetCache().GetItemSubResourceCount(indices[0]) - 1;
      indices[1] = alflib::Clamp(indices[1], 0, s32(maxSubResource));
      ImGui::SliderInt("Sub-resource", &indices[1], 0, maxSubResource);

      Vector2F texMin, texMax;
      gameClient.GetCache().GetTextureCoordinatesForItem(
        indices[0], indices[1], texMin, texMax);
      const auto& atlasTexture = gameClient.GetCache().GetItemAtlasTexture();
      Vector2F size((texMax.x - texMin.x) * atlasTexture->GetWidth() * 10,
                    (texMax.y - texMin.y) * atlasTexture->GetHeight() * 10);
      ImGui::Image(reinterpret_cast<ImTextureID>(atlasTexture->GetID()),
                   ImVec2(size.x, size.y),
                   ImVec2(texMin.x, texMin.y),
                   ImVec2(texMax.x, texMax.y),
                   ImVec4(1, 1, 1, 1),
                   ImVec4(0, 0, 0, 1));

      ImGui::TreePop();
    }

    // Display item atlas
    if (ImGui::TreeNode("Item atlas")) {

      static s32 atlasSize[2] = { 256, 256 };
      static bool uniformAtlasSize = true;
      ImGui::Checkbox("Uniform size", &uniformAtlasSize);
      ImGui::SliderInt("Width", &atlasSize[0], 64, 2048);
      ImGui::SliderInt("Height", &atlasSize[1], 64, 2048);
      if (uniformAtlasSize) {
        atlasSize[1] = atlasSize[0];
      }

      const auto& atlasTexture = gameClient.GetCache().GetItemAtlasTexture();
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
ShowWorldDebug(GameClient& gameClient)
{
  if (ImGui::CollapsingHeader("World")) {
    static char8 nameBuffer[512];
    ImGui::InputText("File name", nameBuffer, 512);
    ImGui::SameLine();
    if (ImGui::Button("Save")) {
      Path path = Path{ "./res" }.Join(Path{ nameBuffer });
      gameClient.GetWorld().Save(path, true);
    }
    if (ImGui::Button("Load")) {
      Path path = Path{ "./res" }.Join(Path{ nameBuffer });
      gameClient.GetWorld().Load(path);
    }
  }
}

// -------------------------------------------------------------------------- //

void
ShowNetworkDebug(GameClient& gameClient)
{
  World& world = gameClient.GetWorld();
  Network<Side::kClient>& network = world.GetNetwork();
  auto& chat = world.GetChat();
  static dutil::Stopwatch sw{};

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

    ImGui::Spacing();

    // ============================================================ //
    // Player List
    // ============================================================ //

    if (ImGui::TreeNode("player list")) {
      std::string player_list = dlog::Format(
        "{:<5} {:<20} {:<4} {:<4}\n", "Ping", "Name", "CQLc", "CQRm");
      auto& registry = world.GetEntityManager().GetRegistry();
      const auto view = registry.view<PlayerData>();
      for (const auto entity : view) {
        const auto pd = view.get(entity);
        player_list += dlog::Format("{:<5} {:<20} {:.1f}  {:.1f}\n",
                                    pd.ping,
                                    pd.name,
                                    pd.con_quality_local / 255.0f,
                                    pd.con_quality_remote / 255.0f);
      }

      ImGui::TextUnformatted(player_list.c_str());

      ImGui::TreePop();
    }

    // ============================================================ //
    // Packet Handler
    // ============================================================ //

    if (ImGui::TreeNode("packet handler")) {
      static std::string packet_types =
        network.GetPacketHandler().PacketTypesToString();
      if (sw.now_ms() > 3000) {
        packet_types = network.GetPacketHandler().PacketTypesToString();
      }
      ImGui::TextUnformatted(packet_types.c_str());

      ImGui::TreePop();
    }

    // ============================================================ //
    // Info
    // ============================================================ //

    if (ImGui::TreeNode("Info")) {
      auto& network = world.GetNetwork();
      auto status = network.GetConnectionStatus();
      if (status) {
        std::string info = dlog::Format("Connection Quality Local: {:.2f}\n"
                                        "Connection Quality Remote: {:.2f}\n"
                                        "Bytes/s Capacity Estimation: {}\n",
                                        status->m_flConnectionQualityLocal,
                                        status->m_flConnectionQualityRemote,
                                        status->m_nSendRateBytesPerSecond);
        ImGui::TextUnformatted(info.c_str());

        constexpr u32 kCount = 90;
        static float ping[kCount] = {};
        static float out_bytes[kCount] = {};
        static float out_packets[kCount] = {};
        static float in_bytes[kCount] = {};
        static float in_packets[kCount] = {};

        const auto ShiftInsertArray =
          [](float* arr, const u32 arrlen, const float value) {
            for (u32 i = arrlen; i-- > 1;) {
              arr[i] = arr[i - 1];
            }
            arr[0] = value;
          };

        if (sw.now_ms() > 1000) {
          sw.Start();
          ShiftInsertArray(ping, kCount, status->m_nPing);
          ShiftInsertArray(out_bytes, kCount, status->m_flOutBytesPerSec);
          ShiftInsertArray(out_packets, kCount, status->m_flOutPacketsPerSec);
          ShiftInsertArray(in_bytes, kCount, status->m_flInBytesPerSec);
          ShiftInsertArray(in_packets, kCount, status->m_flInPacketsPerSec);
        }

        const auto FindMax = [](float* arr, const u32 arrlen) {
          float max = arr[0];
          for (u32 i = 1; i < arrlen; i++) {
            if (arr[i] > max)
              max = arr[i];
          }
          return max;
        };

        const auto ping_current =
          dlog::Format("currently {} ms ping", status->m_nPing);
        const float ping_max = FindMax(ping, kCount);
        ImGui::PlotLines("ping ms",
                         ping,
                         kCount,
                         0,
                         ping_current.c_str(),
                         0,
                         ping_max,
                         ImVec2(0, 80));

        const auto out_bytes_current =
          dlog::Format("currently {:.2f} bytes/s", status->m_flOutBytesPerSec);
        const float out_bytes_max = FindMax(out_bytes, kCount);
        ImGui::PlotLines("out bytes/s",
                         out_bytes,
                         kCount,
                         0,
                         out_bytes_current.c_str(),
                         0,
                         out_bytes_max,
                         ImVec2(0, 80));

        const auto out_packets_current = dlog::Format(
          "currently {:.2f} packets/s", status->m_flOutPacketsPerSec);
        const float out_packets_max = FindMax(out_packets, kCount);
        ImGui::PlotLines("out packets/s",
                         out_packets,
                         kCount,
                         0,
                         out_packets_current.c_str(),
                         0,
                         out_packets_max,
                         ImVec2(0, 80));

        const auto in_bytes_current =
          dlog::Format("currently {:.2f} bytes/s", status->m_flOutBytesPerSec);
        const float in_bytes_max = FindMax(in_bytes, kCount);
        ImGui::PlotLines("in bytes/s",
                         in_bytes,
                         kCount,
                         0,
                         in_bytes_current.c_str(),
                         0,
                         in_bytes_max,
                         ImVec2(0, 80));

        const auto in_packets_current = dlog::Format(
          "currently {:.2f} packets/s", status->m_flInPacketsPerSec);
        const float in_packets_max = FindMax(in_packets, kCount);
        ImGui::PlotLines("in packets/s",
                         in_packets,
                         kCount,
                         0,
                         in_packets_current.c_str(),
                         0,
                         in_packets_max,
                         ImVec2(0, 80));
      }

      ImGui::TreePop();
    }

    // ============================================================ //
    // Chat
    // ============================================================ //

    if (ImGui::TreeNode("Chat")) {

      const auto maybe_our_player_data = network.GetOurPlayerData();
      if (maybe_our_player_data) {
        ImGui::Text("Player Name: %s",
                    (*maybe_our_player_data)->name.GetUTF8());
      }

      constexpr std::size_t buflen = 50;
      static char8 buf[buflen] = "Rully";
      if (world.GetNetwork().GetConnectionState() ==
          ConnectionState::kConnected) {

        if (ImGui::InputText("New Player Name",
                             buf,
                             buflen,
                             ImGuiInputTextFlags_EnterReturnsTrue) &&
            buf[0] != 0) {
          // set our new name
          PlayerData player_data = **world.GetNetwork().GetOurPlayerData();
          player_data.name = String(buf);

          // update locally
          auto& registry = world.GetEntityManager().GetRegistry();
          const bool ok = system::Replace(registry, player_data);
          if (!ok) {
            DLOG_ERROR("failed to replace our PlayerData, disconnecting");
            world.GetNetwork().Disconnect();
          } else {
            // send update to server
            Packet packet{};
            world.GetNetwork().GetPacketHandler().BuildPacketHeader(
              packet, PacketHeaderStaticTypes::kPlayerUpdate);
            auto mw = packet.GetMemoryWriter();
            mw->Write(player_data);
            mw.Finalize();
            world.GetNetwork().PacketBroadcast(packet);
          }

          ImGui::SetKeyboardFocusHere(-1);
        }
      }

      ImGui::Spacing();

      if (world.GetNetwork().GetConnectionState() ==
          ConnectionState::kConnected) {
        constexpr std::size_t textlen = 128;
        static char8 text[textlen] = "";
        if (ImGui::InputText("Chat Message",
                             text,
                             textlen,
                             ImGuiInputTextFlags_EnterReturnsTrue) &&
            text[0] != 0) {
          game::ChatMessage msg{};
          msg.msg = String(text);
          msg.type = game::ChatType::kSay;
          msg.uuid_from = (*world.GetNetwork().GetOurPlayerData())->uuid;
          if (!chat.SendMessage(msg)) {
            DLOG_WARNING("failed to send chat message");
          }
          text[0] = '\0';

          ImGui::SetKeyboardFocusHere(-1);
        }
      }

      ImGui::InputTextMultiline("##source",
                                const_cast<char8*>(chat.GetDebug().GetUTF8()),
                                chat.GetDebug().GetSize(),
                                ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16),
                                ImGuiInputTextFlags_AllowTabInput |
                                  ImGuiInputTextFlags_ReadOnly);

      ImGui::TreePop();
    }
  }
}

// -------------------------------------------------------------------------- //

void
ShowPlayerDebug(GameClient& gameClient)
{
  World& world = gameClient.GetWorld();
  Network<Side::kClient>& network = world.GetNetwork();

  if (ImGui::CollapsingHeader("Player")) {
    auto maybe_entity = network.GetOurPlayerEntity();
    if (maybe_entity) {
      Moveable& m =
        world.GetEntityManager().GetRegistry().get<Moveable>(*maybe_entity);

      std::string info = dlog::Format(
        "1 tile is {:.3f} meter\n1 pixel is {:.3f} meter\n"
        "meter: ({:<6.1f}, {:<6.1f})\npixels: ({:<6.1f}, {:<6.1f})\n"
        "tiles: ({:<6}, {:<6})\nh velocity: {}\nv velocity: {}\n"
        "width,height: {:.2f},{:.2f}",
        game::kTileInMeters,
        game::kPixelInMeter,
        m.position.x,
        m.position.y,
        MeterToPixel(m.position.x),
        MeterToPixel(m.position.y),
        MeterToTile(m.position.x),
        MeterToTile(m.position.y),
        m.horizontal_velocity,
        m.vertical_velocity,
        m.width,
        m.height);

      ImGui::TextUnformatted(info.c_str());
      ImGui::SliderFloat(
        "velocity input", &m.velocity_input, 0.0f, 1000.0f, "%.2f");
      ImGui::SliderFloat(
        "velocity max", &m.velocity_max, 0.0f, 1000.0f, "%.2f");
      ImGui::SliderFloat(
        "velocity jump", &m.velocity_jump, 0.0f, 1000.0f, "%.2f");
      ImGui::InputFloat("x position (meter)", &m.position.x);
      ImGui::InputFloat("y position (meter)", &m.position.y);
    } else {
      ImGui::TextUnformatted("Connect to server to see player information.");
    }
  }
}

}
