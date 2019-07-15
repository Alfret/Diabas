#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <tsl/robin_map.h>

#include "core/types.hpp"
#include "game/wall/wall.hpp"

// ========================================================================== //
// WallRegistry Declaration
// ========================================================================== //

namespace dib::game {

/** Class that represents a registry of wall types **/
class WallRegistry
{
public:
  /** Type for wall-tile IDs **/
  using WallID = u16;

private:
  /** List of registered walls. Indices in the array are their IDs **/
  std::vector<Wall*> mWalls;
  /** Map of tiles from their register-key to their IDs **/
  tsl::robin_map<String, WallID> mWallRegistryMap;
  /** Map of tiles from themselves to their IDs **/
  tsl::robin_map<Wall*, WallID> mWallMap;

  /** Builtin walls **/
  struct
  {
    /** Invalid wall **/
    Wall* wallInvalid = nullptr;
    /** Missing wall **/
    Wall* wallMissing = nullptr;
  } mBuiltin;

public:
  /** Construct wall registry with only builtin walls registered **/
  WallRegistry();

  /** Destruct **/
  ~WallRegistry();

  /** Register a wall with the registry. If the wall has already been registered
   * then the function returns false. This function will combine the modId and
   * the registryKey to create the actual key that is stored in the registry **/
  bool RegisterWall(const String& modId, const String& registryKey, Wall* wall);

  /** Register a wall with the registry. If the wall has already been registered
   * then the function returns false **/
  bool RegisterWall(const String& registryKey, Wall* wall);

  /** Returns the wall for a given ID **/
  [[nodiscard]] Wall* GetWall(WallID id) const;

  /** Returns the wall for a given registry key **/
  [[nodiscard]] Wall* GetWall(const String& registryKey) const;

  /** Returns the wall ID for a given registry key **/
  [[nodiscard]] WallID GetWallID(const String& registryKey) const;

  /** Returns the wall ID for a given wall **/
  [[nodiscard]] WallID GetWallID(Wall* wall) const;

  /** Returns the list of all registered walls. Indices are the IDs of the
   * wall **/
  [[nodiscard]] const std::vector<Wall*>& GetWalls() const { return mWalls; }

  /** Returns the map of registered walls. Keys are the registry names and the
   * value is the ID of the wall. This ID can then be used to index the vector
   * returned from 'GetWalls()'**/
  [[nodiscard]] const tsl::robin_map<String, WallID>& GetRegistryMap() const
  {
    return mWallRegistryMap;
  }

public:
  /** Returns the WallRegistry instance **/
  static WallRegistry& Instance();

  /** Create a registry key from the name of a mod and the mod-specific key **/
  static String CreateRegistryKey(const String& mod, const String& key);
};

}