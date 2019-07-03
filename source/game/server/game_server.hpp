#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "app/server/app_server.hpp"
#include "game/world.hpp"
#include "game/mods/loader.hpp"
#include "game/server/cli_input.hpp"
#include "game/item/item_registry.hpp"
#include "script/env.hpp"

// ========================================================================== //
// Client Declaration
// ========================================================================== //

namespace dib::game {

/** Game server **/
class GameServer : public app::AppServer
{
private:
  /** Tile registry **/
  TileRegistry mTileRegistry;
  /** Item registry **/
  ItemRegistry mItemRegistry;
  /** Game world **/
  World mWorld;

  /** Script environment **/
  script::Environment mScriptEnvironment;
  /** Mod loader **/
  game::ModLoader mModLoader;

  /** Input handler **/
  CLIInputHandler mCLI;

public:
  /** Construct game server **/
  explicit GameServer(const AppServer::Descriptor& descriptor);

  /** Update server **/
  void Update(f64 delta) override;

  /** Returns the world **/
  World& GetWorld() { return mWorld; }

  /** Returns the world **/
  const World& GetWorld() const { return mWorld; }

  /** Returns the script environment **/
  script::Environment& GetScriptEnvironment() { return mScriptEnvironment; }

  /** Returns the script environment **/
  const script::Environment& GetScriptEnvironment() const
  {
    return mScriptEnvironment;
  }

  /** Returns the mod loader **/
  ModLoader& GetModLoader() { return mModLoader; }

  /** Returns the mod loader **/
  const ModLoader& GetModLoader() const { return mModLoader; }

private:
  /** Register all available commands **/
  void RegisterCommands();
};

}