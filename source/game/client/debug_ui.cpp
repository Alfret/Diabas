#include "game/client/debug_ui.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/memory.hpp"
#include "app/client/imgui/imgui.h"
#include "game/client/game_client.hpp"
#include "game/client/player_data_storage.hpp"
#include "game/tile/tile_registry.hpp"
#include "script/util.hpp"
#include "game/ecs/systems/generic_system.hpp"
#include <dutil/stopwatch.hpp>

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

    f64 vmUsage, residentSet;
    core::GetVirtualMemoryUsage(vmUsage, residentSet);
    ImGui::BulletText("Memory usage (MB): %f", residentSet / 1000.0);
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
  TileRegistry& tileRegistry = gameClient.GetTileRegistry();
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
ShowItemDebug(GameClient& gameClient)
{
  if (ImGui::CollapsingHeader("Items")) {
    // Display item atlas
    if (ImGui::TreeNode("Item atlas")) {

      static s32 atlasSize[2] = { 256, 256 };
      static bool uniformAtlasSize = true;
      ImGui::Checkbox("Uniform size", &uniformAtlasSize);
      ImGui::SliderInt("Width", &atlasSize[0], 64, 2048);
      ImGui::SliderInt("Height", &atlasSize[1], 64, 2048);
      atlasSize[0] += ImGui::GetScrollY() * 10;
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
ShowNetworkDebug(GameClient& gameClient)
{
  World& world = gameClient.GetWorld();
  Network<Side::kClient>& network = world.GetNetwork();
  auto& chat = world.GetChat();

  ImGui::ShowTestWindow();

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

        static dutil::Stopwatch sw{};
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
        ImGui::InputText("New Player Name", buf, buflen);
        if (ImGui::Button("Set Name") && buf[0] != 0) {
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
        }
      }

      ImGui::Spacing();

      if (world.GetNetwork().GetConnectionState() ==
          ConnectionState::kConnected) {
        constexpr std::size_t textlen = 128;
        static char8 text[textlen] = "";
        ImGui::InputText("Chat Message", text, textlen);
        if (ImGui::Button("Send Chat Message") && text[0] != 0) {
          game::ChatMessage msg{};
          msg.msg = String(text);
          msg.type = game::ChatType::kSay;
          msg.uuid_from = (*world.GetNetwork().GetOurPlayerData())->uuid;
          if (!chat.SendMessage(msg)) {
            DLOG_WARNING("failed to send chat message");
          }
          text[0] = '\0';
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

}
