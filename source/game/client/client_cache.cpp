#include "game/client/client_cache.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/constants.hpp"
#include "game/tile/tile_registry.hpp"

// ========================================================================== //
// ClientCache Implementation
// ========================================================================== //

namespace dib::game {

void
ClientCache::BuildTileAtlas(const TileRegistry& tileRegistry)
{
  // List of all images to pack and their keys
  alflib::ArrayList<alflib::Image> packImages;
  alflib::ArrayList<String> packNames;

  // Go through each registered tile
  u32 tileIndex = 0;
  for (const Tile* tile : tileRegistry.GetTiles()) {
    // Load the resource
    const ResourcePath& resourcePath = tile->GetResourcePath();
    alflib::Image resourceImage;
    resourceImage.Load(resourcePath.GetPath());

    // Add image for each sub-resource
    u32 resourceCountX = resourceImage.GetWidth() / TILE_SIZE;
    u32 resourceCountY = resourceImage.GetHeight() / TILE_SIZE;
    u32 tileSubIndex = 0;
    for (u32 y = 0; y < resourceCountY; y++) {
      for (u32 x = 0; x < resourceCountX; x++) {
        alflib::Image subResourceImage;
        subResourceImage.Create(TILE_SIZE, TILE_SIZE);
        subResourceImage.Blit(resourceImage,
                              0,
                              0,
                              x * TILE_SIZE,
                              y * TILE_SIZE,
                              TILE_SIZE,
                              TILE_SIZE);
        packImages.AppendEmplace(std::move(subResourceImage));
        packNames.AppendEmplace(
          CreateAtlasKey("tile", tileIndex, tileSubIndex));
        tileSubIndex++;
      }
    }
    std::vector<AtlasRegion> resourceVector;
    resourceVector.resize(tileSubIndex);
    mTileResources.emplace_back(std::move(resourceVector));
    tileIndex++;
  }

  // Create atlas
  u32 atlasDimension =
    u32(std::ceil(alflib::SquareRoot(f32(packImages.GetSize())))) * TILE_SIZE;
  mTileAtlas.Build(packImages, packNames, atlasDimension, atlasDimension);
  mTileAtlas.GetImage().Save(Path{ "./res/tiles/atlas.tga" }, true);

  // Retrieve and store regions in list
  for (u32 index = 0; index < tileRegistry.GetTiles().size(); index++) {
    u32 subResourceCount = mTileResources[index].size();
    for (u32 subIndex = 0; subIndex < subResourceCount; subIndex++) {
      AtlasRegion& region = mTileResources[index][subIndex];

      String key = CreateAtlasKey("tile", index, subIndex);
      alflib::ImageAtlasRegion imageRegion = mTileAtlas.GetRegion(key);
      region.x = imageRegion.x;
      region.y = imageRegion.y;
    }
  }

  // Create atlas texture
  mTileAtlasTexture = std::make_shared<graphics::Texture>("TileAtlas");
  mTileAtlasTexture->Load(mTileAtlas.GetImage());
}

// -------------------------------------------------------------------------- //

const std::vector<ClientCache::AtlasRegion>&
ClientCache::GetSubResources(TileRegistry::TileID id)
{
  return mTileResources[id];
}

// -------------------------------------------------------------------------- //

void
ClientCache::GetTextureCoordinatesForTile(TileRegistry::TileID id,
                                          u32 resourceIndex,
                                          Vector2F& texMin,
                                          Vector2F& texMax)
{
  AtlasRegion& region = mTileResources[id][resourceIndex];
  texMin = Vector2F(f32(region.x) / mTileAtlas.GetWidth(),
                    f32(region.y) / mTileAtlas.GetHeight());
  texMax = Vector2F(f32(region.x + TILE_SIZE) / mTileAtlas.GetWidth(),
                    f32(region.y + TILE_SIZE) / mTileAtlas.GetHeight());
}

// -------------------------------------------------------------------------- //

String
ClientCache::CreateAtlasKey(const String& type,
                            u32 resourceIndex,
                            u32 subResourceIndex)
{
  return type + "_" + String::ToString(resourceIndex) + "_" +
         String::ToString(subResourceIndex);
}

}