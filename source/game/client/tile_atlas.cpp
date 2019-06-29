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
  alflib::ArrayList<String> names;
  s32 index = 0;
  for (auto& tile : tileManager.GetTiles()) {
    const std::vector<ResourcePath>& resources = tile.tile->GetResourcePaths();
    for (auto& resource : resources) {

      if (resource.GetType() == ResourceType::kTexture) {
        alflib::Image image;
        image.Load(resource.GetPath());
        images.AppendEmplace(std::move(image));
        names.Append(resource.GetPath().GetPathString());
      } else if (resource.GetType() == ResourceType::kTextureTileRug) {
        AddRugPattern(resource, images, names);
      }
    }
    index++;
  }

  // Create atlas
  u32 dim = std::ceil(std::sqrt(images.GetSize())) * TileManager::TILE_SIZE;
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

// -------------------------------------------------------------------------- //

void
TileAtlas::AddRugPattern(const ResourcePath& resource,
                         alflib::ArrayList<alflib::Image>& images,
                         alflib::ArrayList<String>& names)
{
  alflib::Image atlas;
  atlas.Load(resource.GetPath());

  for (u32 i = 0; i < 16; i++) {
    alflib::Image image;
    image.Create(16, 16);

    u32 x = i % 4;
    u32 y = std::floor(i / 4);
    image.Blit(atlas, 0, 0, x * 16, y * 16, 16, 16);

    images.AppendEmplace(std::move(image));

    const Path& resPath = resource.GetResourcePath();
    String pathName =
      resPath.GetDirectory().Join(resPath.GetBaseName()).GetPathString();
    pathName += "_" + String::ToString(i) + resPath.GetExtensionString();
    names.Append(pathName);
  }

  int y = 0;
}

}