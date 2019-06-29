#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <alflib/graphics/image_atlas.hpp>

#include "game/tile/tile_manager.hpp"
#include "graphics/texture.hpp"

// ========================================================================== //
// TileAtlas Declaration
// ========================================================================== //

namespace dib::game {

/** Class representing the texture atlas with all loaded tiles on **/
class TileAtlas
{
private:
  /** Atlas **/
  alflib::ImageAtlas<> mTileAtlas;
  /** Atlas texture **/
  std::shared_ptr<graphics::Texture> mAtlasTexture;

public:
  /** Construct tile atlas **/
  TileAtlas() = default;

  /** Build atlas **/
  void Build(const game::TileManager& tileManager);

  /** Returns the UV coordinates for a ResourcePath on the atlas **/
  void GetTextureCoordinates(const ResourcePath& path,
                             Vector2F& texMin,
                             Vector2F& texMax);

  /** Returns the atlas texture **/
  std::shared_ptr<graphics::Texture> GetTexture() const
  {
    return mAtlasTexture;
  }

private:
  void AddRugPattern(const ResourcePath& resource,
                     alflib::ArrayList<alflib::Image>& images,
                     alflib::ArrayList<String>& names);
};

}