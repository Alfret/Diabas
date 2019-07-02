#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"
#include "core/macros.hpp"
#include "game/world_pos.hpp"
#include "game/resource.hpp"
#include "game/item/tool.hpp"

// ========================================================================== //
// Tile Declaration
// ========================================================================== //

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(World);

/** Class representing a tile in the world. Walls and wires are not tiles
 *
 * Each tile has a large set of properties that determine how the block is shown
 * and how interactions with it work.
 *
 * Properties:
 * - IsLightEmitter: This property determines if the block is a light emitter.
 *   A light emitter emits light of a given strength and color. The strength and
 *   color are other properties of a tile
 *
 * - LightStrength:
 *
 * - LightColor:
 *
 * - Opacity:
 *
 * - Collision:
 *
 * - Hardness:
 *
 * - MinimumToolLevel:
 *
 * - RequiredToolType:
 *
 * - IsDestructible
 *
 *
 * **/
class Tile
{
public:
  /** Enumeration of collision types. This determines how entities collide with
   * the tile **/
  enum class CollisionType
  {
    /** Full collision **/
    kFull,
    /** No collision **/
    kNone,
    /** Stairs collision **/
    kStairs,
    /** Custom collision. 'Tile::GetAABB()' is called to determine collision **/
  };

protected:
  /** Path to the resource **/
  ResourcePath mResourcePath;
  /** Translation key for the block in general. More can be specified in
   * sub-classes if the block has different keys depending on position **/
  String mTranslationKey;

  /** Whether tile is a light emitter **/
  bool mIsLightEmitter = false;
  /** Light strength **/
  f32 mLightStrength = 0.0f;
  /** Light color **/
  Color mLightColor = Color::WHITE;

  /** Opacity **/
  f32 mOpacity = 1.0f;

  /** Collision type **/
  CollisionType mCollisionType = CollisionType::kFull;

  /** Hardness of the tile **/
  f32 mHardness = 1.0f;
  /** Minimum required tool level **/
  u32 mMinimumToolLevel = 1;
  /** Tool type required to destroy tile **/
  ToolType mRequiredToolType = ToolType::kAll;
  /** Whether the tile is destructible **/
  bool mIsDestructible = true;

public:
  /** Construct a tile by specifying the path to the resource. This resource
   * is actually only loaded on clients and not used on servers.
   *
   * A translation key can also be specified that is used for translation of the
   * name of the block **/
  explicit Tile(ResourcePath resourcePath, String translationKey);

  /** Virtual destructor **/
  virtual ~Tile() = default;

  /** Called when block is activated (right-clicked on client). Returning true
   * means that the block handled the activation **/
  virtual bool OnActivated(World& world, WorldPos pos);

  /** Returns whether or not the tile emits light. If this returns true then the
   * functions 'GetLightStrength' and 'GetLightColor' is also called to
   * determine the strength and color of the emitted light **/
  [[nodiscard]] virtual bool IsLightEmitter(World& world, WorldPos pos);

  /** Set whether the tile is a light emitter. A pointer to the tile itself is
   * returned (used for chaining) **/
  virtual Tile* SetIsLightEmitter(bool isLightEmitter);

  /** Returns the strength of the emitted light. Only called if the tile is a
   * light emitter **/
  [[nodiscard]] virtual f32 GetLightStrength(World& world, WorldPos pos);

  /** Sets the strength of the light that is emitted by the tile. A pointer to
   * the tile itself is returned (used for chaining) **/
  virtual Tile* SetLightStrength(f32 lightStrength);

  /** Returns the color of the emitted light. Only called if the tile is a light
   * emitter **/
  [[nodiscard]] virtual Color GetLightColor(World& world, WorldPos pos);

  /** Sets the color of the light that is emitted by the tile. A pointer to
   * the tile itself is returned (used for chaining) **/
  virtual Tile* SetLightColor(Color lightColor);

  /** Returns the opacity of the tile **/
  [[nodiscard]] virtual f32 GetOpacity(World& world, WorldPos pos);

  /** Sets the opacity of the tile. A pointer to the tile itself is returned
   * (used for chaining) **/
  virtual Tile* SetOpacity(f32 opacity);

  /** Returns the collision type of the tile at the specified location in the
   * world **/
  [[nodiscard]] virtual CollisionType GetCollision(World& world, WorldPos pos);

  /** Sets the collision type of the tile **/
  virtual Tile* SetCollisionType(CollisionType collisionType);

  /** Returns the hardness of the tile at the given position in the world **/
  [[nodiscard]] virtual f32 GetHardness(World& world, WorldPos pos);

  /** Sets the hardness of the tile **/
  virtual Tile* SetHardness(f32 hardness);

  /** Returns the minimum tool level required to destroy the tile **/
  [[nodiscard]] virtual u32 GetMinimumToolLevel();

  /** Sets the minimum tool level required to destroy the tile **/
  virtual Tile* SetMinimumToolLevel(u32 toolLevel);

  /** Returns the tool type(s) required to destroy this tile **/
  [[nodiscard]] virtual ToolType GetRequiredToolType();

  /** Sets the tool type(s) required to destroy this tile **/
  virtual Tile* SetRequiredToolType(ToolType toolType);

  /** Returns whether or not the tile can actually be destroyed **/
  [[nodiscard]] virtual bool IsDestructible();

  /** Sets whether or not the tile can actually be destroyed **/
  virtual Tile* SetIsDestructible(bool isDestructible);

  /** Returns the index of the resource for the tile at a given position in
   * the world. Indices determine which sub-resource in the resource loaded
   * from the resource path that the tile at the position will use. Index 0 is
   * the top-left sub-resource **/
  [[nodiscard]] virtual u32 GetResourceIndex(World& world, WorldPos pos);

  /** Returns the key used for translation of the tile name **/
  [[nodiscard]] virtual const String& GetTranslationKey(World& world,
                                                        WorldPos pos) const;

  /** Returns the key used for translation of the tile name **/
  [[nodiscard]] virtual const String& GetTranslationKey() const;

  /** Returns the resource path of the tile **/
  [[nodiscard]] const ResourcePath& GetResourcePath() const;
};

}