#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <array>

#include "core/types.hpp"
#include "core/macros.hpp"
#include "graphics/camera.hpp"
#include "game/tile/tile_registry.hpp"

// ========================================================================== //
// Terrain Declaration
// ========================================================================== //

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(World);

/** Class that manages the terrain  **/
class Terrain
{
public:
  /** Common terrain sizes **/
  enum class Size
  {
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
    TileRegistry::WallID wall;
    TileRegistry::WireID wire;
    u8 metadata;
    u8 cachedTileSubResource;
    u8 cachedWallSubResource;
    u8 cachedWireSubResource;
  };

private:
  /** Tile registry **/
  const TileRegistry& mTileRegistry;
  /** World **/
  World& mWorld;

  /** Width of terrain **/
  u32 mWidth;
  /** Height of terrain **/
  u32 mHeight;

  /** Terrain cells **/
  Cell* mTerrainCells;

public:
  /** Construct a world of the specified dimensions **/
  Terrain(const TileRegistry& tileRegistry,
          World& world,
          u32 width,
          u32 height);

  /** Construct a world of the specified size **/
  Terrain(const TileRegistry& tileRegistry, World& world, Size size);

  /** Returns the tile at the specified location in the world **/
  Tile* GetTile(WorldPos pos) const;

  /** Returns the ID of the tile at the specified location in the world **/
  TileRegistry::TileID GetTileID(WorldPos pos) const;

  /** Sets the tile at the specified location in the world. The function returns
   * false if the tile could not be placed **/
  bool SetTile(WorldPos pos, Tile* tile);

  /** Sets the tile at the specified location in the world given its ID. The
   * function returns false if the tile could not be placed **/
  bool SetTile(WorldPos pos, TileRegistry::TileID id);

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

  /** Remove the tile at the given location in the world. The removed tile is
   * replaced with the specified tile **/
  void RemoveTile(WorldPos pos, Tile* replacementTile);

  /** Remove the tile at the given location in the world. The removed tile is
   * replaced with the specified tile **/
  void RemoveTile(WorldPos pos, TileRegistry::TileID replacementId);

  /** Update the cache for the tile resources for every tile in the world. This
   * function should not be called often.
   *
   * It should be called after generation of the world to cache the indices, as
   * the world generation functions (Terrain::GenSetTile) does not update the
   * indices.
   */
  void ReCacheResourceIndices();

  /** Returns whether a position is valid **/
  bool IsValidPosition(WorldPos pos);

  /** Returns the metadata for a position in the world **/
  u8 GetMetadata(WorldPos pos);

  /** Sets the metadata for a position in the world **/
  void SetMetadata(WorldPos pos, u8 metadata);

  /** Returns a cell in the world data **/
  Cell& GetCell(WorldPos pos);

  /** Returns the width of the terrain in number of tiles. Zero (0) is left **/
  u32 GetWidth() const { return mWidth; };

  /** Returns the height of the terrain in number of tiles. Zero (0) is
   * bottom **/
  u32 GetHeight() const { return mHeight; };

private:
  /** Initialize the terrain layers **/
  void InitTerrain();

  /** Implementation of 'SetTile' with more flags to determine how it works **/
  bool SetTileAdvanced(WorldPos pos,
                       TileRegistry::TileID id,
                       bool ignoreReplaceCheck = false,
                       bool updateNeighbour = true);

  /** Update the neighbour only if the position is valid in the world **/
  void SafeNeighbourUpdate(WorldPos pos);

  /** Update sub-resource indices **/
  void UpdateCachedIndices(WorldPos pos, bool updateNeighbours = true);
};

}