#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/types.hpp"

// ========================================================================== //
// ResourceType Enumeration
// ========================================================================== //

namespace dib::game {

/** Type of the resource **/
enum class ResourceType : u8
{
  /** Standard texture **/
  kTexture,
  /** Tile atlas (Rug) **/
  kTextureTileRug,
  /** Tile atlas (Blob) **/
  kTextureTileBlob,
};

}

// ========================================================================== //
// ResourcePath
// ========================================================================== //

namespace dib::game {

/** Path to a mod-specific resource **/
class ResourcePath
{
private:
  /** Resource type **/
  ResourceType mType;

  /** Mod ID **/
  String mModID;
  /** Relative resource path from mod directory **/
  Path mResourcePath;

  /** Path relative to game directory **/
  Path mPath;

public:
  /** Create resource by specifying the mod it belongs to and the
   * mod-directory-relative path to the resource **/
  ResourcePath(ResourceType type,
               const String& modId,
               const Path& resourcePath);

  /** Create a resource by specifying the path relative to the game
   * directory **/
  ResourcePath(ResourceType type, const Path& resourcePath);

  /** Returns the type of the resource **/
  ResourceType GetType() const { return mType; }

  /** Returns the id of the mod that the resource belongs to **/
  const String& GetModId() const { return mModID; }

  /** Returns the resource path relative to the mod **/
  const Path& GetResourcePath() const { return mResourcePath; }

  /** Returns the path to the resource relative to the game directory **/
  const Path& GetPath() const;
};

}