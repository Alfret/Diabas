#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/common.hpp"

// ========================================================================== //
// ResourcePath
// ========================================================================== //

namespace dib::game {

/** Path to a mod-specific resource **/
class ResourcePath
{
public:
  /** Flags **/
  enum class Flag : u8
  {
    /** No specific flags **/
    kNone = 0,
    /** Resource has 1-pixel border **/
    kBorder1px = Bit(0)
  };
  DIB_ENUM_CLASS_OPERATORS(friend, Flag, u8);

private:
  /** Mod ID **/
  String mModID;
  /** Relative resource path from mod directory **/
  Path mResourcePath;
  /** Flags **/
  Flag mFlags;

  /** Path relative to game directory **/
  Path mPath;

public:
  /** Create resource by specifying the mod it belongs to and the
   * mod-directory-relative path to the resource **/
  ResourcePath(String modId, Path resourcePath, Flag flags = Flag::kNone);

  /** Create a resource by specifying the path relative to the game
   * directory **/
  explicit ResourcePath(Path resourcePath, Flag flags = Flag::kNone);

  /** Returns the id of the mod that the resource belongs to **/
  [[nodiscard]] const String& GetModId() const { return mModID; }

  /** Returns the resource path relative to the mod **/
  [[nodiscard]] const Path& GetModPath() const { return mResourcePath; }

  /** Returns the flags **/
  [[nodiscard]] Flag GetFlags() const { return mFlags; }

  /** Returns the path to the resource relative to the game directory **/
  [[nodiscard]] const Path& GetPath() const;
};

}