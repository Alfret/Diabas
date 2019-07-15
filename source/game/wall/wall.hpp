#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/macros.hpp"
#include "game/resource.hpp"
#include "game/world_pos.hpp"

// ========================================================================== //
// Wall Declaration
// ========================================================================== //

namespace dib::game {

DIB_FORWARD_DECLARE_CLASS(World);

/** Class that represents a wall tile. These are placed behind the tiles that
 * make up the foreground. **/
class Wall
{
public:
  static constexpr u8 kTop = 1u;
  static constexpr u8 kLeft = 2u;
  static constexpr u8 kRight = 4u;
  static constexpr u8 kBottom = 8u;

protected:
  /** Path to the resource **/
  ResourcePath mResourcePath;
  /** Translation key for the wall in general. More can be specified in
   * sub-classes if the wall has different keys depending on position **/
  String mTranslationKey;

  /** Whether the tile is destructible **/
  bool mIsDestructible = true;
  /** Whether the tile can be replaced **/
  bool mCanBeReplaced = false;

public:
  /** Construct a wall by specifying the path to the resource. This resource
   * is actually only loaded on clients and not used on servers.
   *
   * A translation key can also be specified that is used for translation of the
   * name of the wall **/
  explicit Wall(ResourcePath resourcePath, String translationKey);

  /** Virtual destructor **/
  virtual ~Wall() = default;

  /** Called when this wall has been placed in the world **/
  virtual void OnPlaced(World& world, WorldPos pos);

  /** Called just before a wall is destroyed **/
  virtual void OnDestroyed(World& world, WorldPos pos);

  /** Called when one of the neighbouring eight (8) walls has changed **/
  virtual void OnNeighbourChange(World& world, WorldPos pos);

  /** Returns whether or not the wall can actually be destroyed **/
  [[nodiscard]] virtual bool IsDestructible();

  /** Sets whether or not the wall can actually be destroyed **/
  virtual Wall* SetIsDestructible(bool isDestructible);

  /** Returns whether or not the wall can be replaced **/
  [[nodiscard]] virtual bool CanBeReplaced(World& world, WorldPos pos);

  /** Sets whether or not the wall can be replaced **/
  virtual Wall* SetCanBeReplaced(bool canBeReplaced);

  /** Returns the index of the resource for the wall at a given position in
   * the world. Indices determine which sub-resource in the resource loaded
   * from the resource path that the wall at the position will use. Index 0 is
   * the top-left sub-resource **/
  [[nodiscard]] virtual u32 GetResourceIndex(World& world, WorldPos pos);

  /** Returns the key used for translation of the wall name **/
  [[nodiscard]] virtual const String& GetTranslationKey(World& world,
                                                        WorldPos pos) const;

  /** Returns the key used for translation of the wall name **/
  [[nodiscard]] virtual const String& GetTranslationKey() const;

  /** Returns the resource path of the wall **/
  [[nodiscard]] const ResourcePath& GetResourcePath() const;
};

}