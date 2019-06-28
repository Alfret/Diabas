#include "game.hpp"

#include <dlog.hpp>
#include "network/side.hpp"
#include "app/imgui/imgui.h"
#include "script/util.hpp"
#include "game/player_data_storage.hpp"

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
ShowTileDebug(Game& game,
              game::TileManager& tileManager,
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

    // Change block
    if (game.IsMouseDown(MouseButton::kMouseButtonLeft)) {
      f64 mouseX, mouseY;
      game.GetMousePosition(mouseX, mouseY);
      u32 tileX, tileY;
      terrain.PickTile(game.GetCamera(), mouseX, mouseY, tileX, tileY);
      terrain.SetTile(tile, tileX, tileY, game::Terrain::LAYER_FOREGROUND);
    }

    ImGui::TreePop();
  }
}

// ============================================================ //

static void
ShowNetworkDebug(World& world, const game::Chat& chat)
{
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
        DLOG_INFO("send chat [{}]", text);
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

// -------------------------------------------------------------------------- //

void
UpdateCamera(Game& game, graphics::Camera& camera, f32 delta)
{
  // Move camera
  Vector3F cameraMove(0.0f, 0.0f, 0.0f);
  f32 cameraSpeed = 80;
  if (game.IsKeyDown(Key::kKeyLeft)) {
    cameraMove.x -= cameraSpeed * delta;
  }
  if (game.IsKeyDown(Key::kKeyRight)) {
    cameraMove.x += cameraSpeed * delta;
  }
  if (game.IsKeyDown(Key::kKeyUp)) {
    cameraMove.y += cameraSpeed * delta;
  }
  if (game.IsKeyDown(Key::kKeyDown)) {
    cameraMove.y -= cameraSpeed * delta;
  }
  camera.Move(cameraMove);

  Vector3F minPos = Vector3F(0.0f, 0.0f, 0.0f);
  Vector3F maxPos =
    Vector3F(game.GetTerrain().GetWidth() * game::TileManager::TILE_SIZE -
               camera.GetWidth(),
             game.GetTerrain().GetHeight() * game::TileManager::TILE_SIZE -
               camera.GetHeight(),
             1.0f);
  camera.ClampPosition(minPos, maxPos);
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
  , mTerrain(world_, mTileManager, 8400, 2400)
{
  GetGraphics().SetClearColor(alflib::Color::CORNFLOWER_BLUE);

  DLOG_INFO("running as {}", SideToString(kSide));
  SetupInputCommands();

  // Initialize script environment and load mods
  game::Result modResult = mModLoader.Load(mScriptEnvironment);
  DIB_ASSERT(modResult == game::Result::kSuccess, "Failed to load mods");
  mModLoader.Init(world_);
  mModLoader.RegisterTiles(mTileManager);

  // Create an atlas from all loaded tiles
  mTileManager.CreateAtlas();

  mTerrain.Generate();
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
    ShowNetworkDebug(world_, chat_);
    ShowTileDebug(*this, mTileManager, mTerrain, world_);
  }
  ImGui::End();

  UpdateCamera(*this, mCamera, delta);

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
Game::OnWindowResize(u32 width, u32 height)
{
  mCamera.Resize(width, height);
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
