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
protected:
  /** Path to the resource **/
  ResourcePath mResourcePath;
  /** Translation key for the wall in general. More can be specified in
   * sub-classes if the wall has different keys depending on position **/
  String mTranslationKey;

public:
  /** Construct a wall by specifying the path to the resource. This resource
   * is actually only loaded on clients and not used on servers.
   *
   * A translation key can also be specified that is used for translation of the
   * name of the wall **/
  explicit Wall(ResourcePath resourcePath, String translationKey);

  /** Virtual destructor **/
  virtual ~Wall() = default;

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