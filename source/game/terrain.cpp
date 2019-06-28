#include "game/terrain.hpp"

// ========================================================================== //
// Terrain Implementation
// ========================================================================== //

namespace dib::game {

Terrain::Terrain(World& world, TileManager& tileManager, u32 width, u32 height)
  : mWorld(world)
  , mTileManager(tileManager)
  , mWidth(width)
  , mHeight(height)
{
  mTerrainLayers[0] = new Tile::ID[mWidth * mHeight];
  memset(mTerrainLayers[0], 0, sizeof(Tile::ID) * mWidth * mHeight);

  mTerrainLayers[1] = new Tile::ID[mWidth * mHeight];
  memset(mTerrainLayers[1], 0, sizeof(Tile::ID) * mWidth * mHeight);

  mTerrainLayers[2] = new Tile::ID[mWidth * mHeight];
  memset(mTerrainLayers[2], 0, sizeof(Tile::ID) * mWidth * mHeight);
}

// -------------------------------------------------------------------------- //

void
Terrain::Generate()
{
  std::shared_ptr<Tile> tileAir = mTileManager.GetTile("builtin$air");
  std::shared_ptr<Tile> tileGrass = mTileManager.GetTile("core$grass");
  std::shared_ptr<Tile> tileDirt = mTileManager.GetTile("core$dirt");

  for (u32 y = 0; y < mHeight; y++) {
    for (u32 x = 0; x < mWidth; x++) {

      if (y > 10) {
        SetTile(tileAir, x, y, LAYER_FOREGROUND);
      } else if (y == 10) {
        SetTile(tileGrass, x, y, LAYER_FOREGROUND);
      } else {
        SetTile(tileDirt, x, y, LAYER_FOREGROUND);
      }
    }
  }
}

// -------------------------------------------------------------------------- //

void
Terrain::Draw(graphics::Renderer& renderer, graphics::Camera& camera)
{
  // Retrieve batch
  graphics::SpriteBatch& spriteBatch = renderer.GetSpriteBatch();
  spriteBatch.Begin(&camera);

  // Retrieve block texture
  std::shared_ptr<graphics::Texture> texture = mTileManager.GetTexture();

  // Calculate first tile in camera
  const Vector3F& cameraPos = camera.GetPosition();
  u32 minX = std::floor(camera.GetPosition().x / TileManager::TILE_SIZE);
  if (minX < 0) {
    minX = 0;
  }
  u32 minY = std::floor(camera.GetPosition().y / TileManager::TILE_SIZE);
  if (minY < 0) {
    minY = 0;
  }

  // Calculate number of tiles in camera
  u32 countX = std::ceil(camera.GetWidth() / TileManager::TILE_SIZE);
  if (countX + minX > mWidth) {
    countX = mWidth - minX;
  }
  u32 countY = std::ceil(camera.GetHeight() / TileManager::TILE_SIZE);
  if (countY + minY > mHeight) {
    countY = mHeight - minY;
  }

  // Render each tile
  for (u32 y = minY; y < minY + countY + 1; y++) {
    for (u32 x = minX; x < minX + countX + 1; x++) {
      Vector3F position = Vector3F(x * TileManager::TILE_SIZE - cameraPos.x,
                                   y * TileManager::TILE_SIZE - cameraPos.y,
                                   0.5f);
      alflib::Color tint = alflib::Color::WHITE;
      std::shared_ptr<game::Tile> tile = GetTile(x, y, LAYER_FOREGROUND);
      const game::ResourcePath& resourcePath =
        tile->GetResourcePath(mWorld, x, y);
      Vector2F texMin, texMax;
      mTileManager.GetTextureCoordinates(resourcePath, texMin, texMax);
      spriteBatch.Submit(
        texture,
        position,
        Vector2F(TileManager::TILE_SIZE, TileManager::TILE_SIZE),
        tint,
        texMin,
        texMax);
    }
  }

  // Done rendering
  spriteBatch.End();
}

// -------------------------------------------------------------------------- //

std::shared_ptr<Tile>
Terrain::GetTile(u32 x, u32 y, u32 layer)
{
  Tile::ID id = *GetTerrainTileOffset(x, y, layer);
  return mTileManager.GetTile(id);
}

// -------------------------------------------------------------------------- //

void
Terrain::SetTile(const std::shared_ptr<Tile>& tile, u32 x, u32 y, u32 layer)
{
  *GetTerrainTileOffset(x, y, layer) = tile->GetID();
}

// -------------------------------------------------------------------------- //

void
Terrain::PickTile(const graphics::Camera& camera,
                  f64 mouseX,
                  f64 mouseY,
                  u32& tileX,
                  u32& tileY)
{
  tileX =
    std::floor((camera.GetPosition().x + mouseX) / TileManager::TILE_SIZE);
  tileY =
    std::floor((camera.GetPosition().y + mouseY) / TileManager::TILE_SIZE);
}

// -------------------------------------------------------------------------- //

Tile::ID*
Terrain::GetTerrainTileOffset(u32 x, u32 y, u32 layer)
{
  return mTerrainLayers[layer] + (y * mWidth) + x;
}
}