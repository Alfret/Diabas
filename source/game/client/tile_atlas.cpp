#include "game/client/tile_atlas.hpp"

// ========================================================================== //
// TileAtlas Declaration
// ========================================================================== //

namespace dib::game {

void
TileAtlas::Build(const game::TileManager& tileManager)
{
  // Load images
  alflib::ArrayList<alflib::Image> images;
  images.Resize(tileManager.GetTiles().size());
  alflib::ArrayList<String> names;
  s32 index = 0;
  for (auto& tile : tileManager.GetTiles()) {
    const std::vector<ResourcePath>& resources = tile.tile->GetResourcePaths();
    for (auto& resource : resources) {
      images[index].Load(resource.GetPath());
      names.Append(resource.GetPath().GetPathString());
    }
    index++;
  }

  // Create atlas
  u32 dim = std::ceil(std::sqrt(tileManager.GetTiles().size())) *
            TileManager::TILE_SIZE;
  mTileAtlas.Build(images, names, dim, dim);
  mTileAtlas.GetImage().Save(Path{ "./res/tiles/atlas.tga" }, true);

  // Create atlas texture
  mAtlasTexture = std::make_shared<graphics::Texture>();
  mAtlasTexture->Load(mTileAtlas.GetImage(), "TileAtlas");
}

// -------------------------------------------------------------------------- //

void
TileAtlas::GetTextureCoordinates(const ResourcePath& path,
                                 Vector2F& texMin,
                                 Vector2F& texMax)
{
  alflib::ImageAtlasRegion region =
    mTileAtlas.GetRegion(path.GetPath().GetPathString());

  f32 xOffset = 0.5f / mTileAtlas.GetWidth();
  f32 yOffset = 0.5f / mTileAtlas.GetHeight();

  texMin = Vector2F((f32(region.x) / mTileAtlas.GetWidth()) + xOffset,
                    (f32(region.y) / mTileAtlas.GetHeight()) + yOffset);
  texMax = Vector2F(
    (f32(region.x + region.width) / mTileAtlas.GetWidth()) - xOffset,
    (f32(region.y + region.height) / mTileAtlas.GetHeight()) - yOffset);
}

}