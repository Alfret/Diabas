#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <array>

#include "core/types.hpp"
#include "core/macros.hpp"
#include "graphics/camera.hpp"
#include "game/tile/tile_registry.hpp"
#include "game/wall/wall_registry.hpp"

// ========================================================================== //
// Terrain Declaration
// ========================================================================== //

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(World);

/** Class that manages the terrain  **/
class Terrain
{
  friend World;

public:
  /** Common terrain sizes **/
  enum class Size
  {
    /** Tiny world **/
    kTiny,
    /** Small world **/
    kSmall,
    /** Normal world size **/
    kNormal,
    /** Large world size **/
    kLarge,
    /** Huge world size **/
    kHuge
  };

  /** Cell in the world **/
  struct Cell
  {
    TileRegistry::TileID tile;
    WallRegistry::WallID wall;
    // TileRegistry::WireID wire;
    u8 metadata;
  };

  /** Change listener **/
  class ChangeListener
  {
  public:
    /** Called when the terrain has been resized **/
    virtual void OnResize(u32 width, u32 height) = 0;

    /** Called when a tile in the world changed **/
    virtual void OnTileChanged(WorldPos pos) = 0;

    /** Called when a wall in the world changed **/
    virtual void OnWallChanged(WorldPos pos) = 0;
  };

private:
  /** World **/
  World* mWorld;

  /** Width of terrain **/
  u32 mWidth;
  /** Height of terrain **/
  u32 mHeight;

  /** Terrain cells **/
  Cell* mTerrainCells = nullptr;

  /** Change listeners **/
  std::vector<ChangeListener*> mChangeListeners;

public:
  /** Construct a world of the specified dimensions **/
  Terrain(World* world, u32 width, u32 height);

  /** Construct a world of the specified size **/
  Terrain(World* world, Size size);

  /** Move-constructor **/
  Terrain(Terrain&& other) noexcept;

  /** Destruct **/
  ~Terrain();

  /** Move-assignment **/
  Terrain& operator=(Terrain&& other) noexcept;

  /** Returns the tile at the specified location in the world **/
  [[nodiscard]] Tile* GetTile(WorldPos pos) const;

  /** Returns the ID of the tile at the specified location in the world **/
  [[nodiscard]] TileRegistry::TileID GetTileID(WorldPos pos) const;

  /** Returns the wall at the specified location in the world **/
  [[nodiscard]] Wall* GetWall(WorldPos pos) const;

  /** Returns the ID of the wall at the specified location in the world **/
  [[nodiscard]] WallRegistry::WallID GetWallID(WorldPos pos) const;

  /** Sets the tile at the specified location in the world. The function returns
   * false if the tile could not be placed **/
  bool SetTile(WorldPos pos, Tile* tile);

  /** Sets the tile at the specified location in the world given its ID. The
   * function returns false if the tile could not be placed **/
  bool SetTile(WorldPos pos, TileRegistry::TileID id);

  /** Sets the wall at the specified location in the world. The function returns
   * false if the wall could not be placed **/
  bool SetWall(WorldPos pos, Wall* wall);

  /** Sets the wall at the specified location in the world given its ID. The
   * function returns false if the wall could not be placed **/
  bool SetWall(WorldPos pos, WallRegistry::WallID id);

  /** Set the tile at the given position in the world during generation. It's
   * important that the function 'Terrain::SetTile' is used during game-play
   * instead. The reason is that this functions skips a lot of work to make
   * generation happen faster, though this also requires the entire world to be
   * re-cached afterwards
   *
   * Also note that multi-tile structures cannot be placed with this function!
   * However during placement of sub-tiles this function SHOULD be used
   *
   * Also note that this function ignores whether or not the current tile at the
   * position can be replaced.
   */
  void GenSetTile(WorldPos pos, Tile* tile, bool updateNeighbours = false);

  /** Set the tile at the given position in the world during generation. It's
   * important that the function 'Terrain::SetTile' is used during game-play
   * instead. The reason is that this functions skips a lot of work to make
   * generation happen faster, though this also requires the entire world to be
   * re-cached afterwards
   *
   * Also note that multi-tile structures cannot be placed with this function!
   * However during placement of sub-tiles this function SHOULD be used
   *
   * Also note that this function ignores whether or not the current tile at the
   * position can be replaced.
   */
  void GenSetTile(WorldPos pos,
                  TileRegistry::TileID id,
                  bool updateNeighbours = false);

  void GenSetWall(WorldPos pos, Wall* wall, bool updateNeighbours = false);

  void GenSetWall(WorldPos pos,
                  WallRegistry::WallID id,
                  bool updateNeighbours = false);

  /** Remove the tile at the given location in the world. The removed tile is
   * replaced with the specified tile **/
  void RemoveTile(WorldPos pos, Tile* replacementTile);

  /** Remove the tile at the given location in the world. The removed tile is
   * replaced with the specified tile **/
  void RemoveTile(WorldPos pos, TileRegistry::TileID replacementId);

  /** Resize the terrain. This throws away all old data **/
  void Resize(u32 width, u32 height);

  /** Returns whether a position is valid **/
  bool IsValidPosition(WorldPos pos);

  /** Returns the metadata for a position in the world **/
  u8 GetMetadata(WorldPos pos);

  /** Sets the metadata for a position in the world **/
  void SetMetadata(WorldPos pos, u8 metadata);

  /** Register change listener **/
  void RegisterChangeListener(ChangeListener* changeListener);

  /** Unregister change listener **/
  void UnregisterChangeListener(ChangeListener* changeListener);

  /** Returns a cell in the world data **/
  Cell& GetCell(WorldPos pos);

  /** Returns the width of the terrain in number of tiles. Zero (0) is left **/
  [[nodiscard]] u32 GetWidth() const { return mWidth; };

  /** Returns the height of the terrain in number of tiles. Zero (0) is
   * bottom **/
  [[nodiscard]] u32 GetHeight() const { return mHeight; };

private:
  /** Initialize the terrain layers **/
  void InitTerrain();

  /** Implementation of 'SetTile' with more flags to determine how it works **/
  bool SetTileAdvanced(WorldPos pos,
                       TileRegistry::TileID id,
                       bool ignoreReplaceCheck = false,
                       bool updateNeighbour = true);

  /** Update the neighbour only if the position is valid in the world **/
  void SafeNeighbourTileUpdate(WorldPos pos);

  /** Update sub-resource indices **/
  void UpdateCachedTileIndices(WorldPos pos, bool updateNeighbours = true);

  /** Implementation of 'SetWall' with more flags to determine how it works **/
  bool SetWallAdvanced(WorldPos pos,
                       WallRegistry::WallID id,
                       bool ignoreReplaceCheck = false,
                       bool updateNeighbour = true);

  /** Update the neighbour only if the position is valid in the world **/
  void SafeNeighbourWallUpdate(WorldPos pos);

  /** Update sub-resource indices **/
  void UpdateCachedWallIndices(WorldPos pos, bool updateNeighbours = true);
};

}