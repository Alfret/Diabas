#include "game/tile/tile_manager.hpp"

// ========================================================================== //
// TileManager Implementation
// ========================================================================== //

namespace dib::game {

TileManager::TileManager()
{
  RegisterBuiltinTiles();
}

// -------------------------------------------------------------------------- //

TileManager::Result
TileManager::RegisterTile(const String& modId,
                          const String& tileId,
                          const std::shared_ptr<Tile>& tile)
{
  // Check that ID only contain valid characters
  if (tileId.IndexOf('$') != -1) {
    return Result::kInvalidTileId;
  }

  // Store tile
  String identifier = modId + "$" + tileId;
  if (mTileMap.count(identifier) > 0) {
    return Result::kTileExists;
  }
  mTiles.push_back(TileEntry{ tile, identifier });
  Tile::ID numId = mTiles.size() - 1;
  mTileMap[identifier] = numId;
  tile->mID = numId;
  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

void
TileManager::CreateAtlas()
{
  // Load images
  alflib::ArrayList<alflib::Image> images;
  images.Resize(mTiles.size());
  alflib::ArrayList<String> names;
  s32 index = 0;
  for (auto& tile : mTiles) {
    const std::vector<ResourcePath>& resources = tile.tile->GetResourcePaths();
    for (auto& resource : resources) {
      images[index].Load(resource.GetPath());
      names.Append(resource.GetPath().GetPathString());
    }
    index++;
  }

  // Create atlas
  mTileAtlas.Build(images, names, 512, 512);
  mTileAtlas.GetImage().Save(Path{ "./res/tiles/atlas.tga" }, true);

  // Create atlas texture
  mAtlasTexture = std::make_shared<graphics::Texture>();
  mAtlasTexture->Load(mTileAtlas.GetImage(), "TileAtlas");
}

// -------------------------------------------------------------------------- //

std::shared_ptr<Tile>
TileManager::GetTile(const String& id)
{
  auto it = mTileMap.find(id);
  if (it != mTileMap.end()) {
    return mTiles[it->second].tile;
  }
  return std::shared_ptr<Tile>(nullptr);
}

// -------------------------------------------------------------------------- //

std::shared_ptr<Tile>
TileManager::GetTile(const String& modId, const String& tileId)
{
  String identifier = modId + "$" + tileId;
  return GetTile(identifier);
}

// -------------------------------------------------------------------------- //

std::shared_ptr<Tile>
TileManager::GetTile(Tile::ID id)
{
  return mTiles[id].tile;
}

// -------------------------------------------------------------------------- //

void
TileManager::GetTextureCoordinates(const ResourcePath& path,
                                   Vector2F& texMin,
                                   Vector2F& texMax)
{
  alflib::ImageAtlasRegion region =
    mTileAtlas.GetRegion(path.GetPath().GetPathString());

  texMin = Vector2F(f32(region.x) / mTileAtlas.GetWidth(),
                    f32(region.y) / mTileAtlas.GetHeight());
  texMax = Vector2F(f32(region.x + region.width) / mTileAtlas.GetWidth(),
                    f32(region.y + region.height) / mTileAtlas.GetHeight());
}

// -------------------------------------------------------------------------- //

void
TileManager::RegisterBuiltinTiles()
{
  auto tile =
    std::make_shared<Tile>(ResourcePath{ Path{ "./res/tiles/air_tile.tga" } });
  RegisterTile("builtin", "air", tile);
}

}