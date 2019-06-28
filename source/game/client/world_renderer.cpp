#include "game/client/world_renderer.hpp"

// ========================================================================== //
// TerrainRenderer Implementation
// ========================================================================== //

namespace dib::game {

void
WorldRenderer::RenderWorld(graphics::Renderer& renderer,
                           TileAtlas& tileAtlas,
                           World& world,
                           const graphics::Camera& camera)
{
  Terrain& terrain = world.GetTerrain();

  // Retrieve batch and begin
  graphics::SpriteBatch& spriteBatch = renderer.GetSpriteBatch();
  spriteBatch.Begin(&camera);

  // Calculate first tile in camera
  const Vector3F& cameraPos = camera.GetPosition();
  u32 minX = std::floor(camera.GetPosition().x / game::TileManager::TILE_SIZE);
  if (minX < 0) {
    minX = 0;
  }
  u32 minY = std::floor(camera.GetPosition().y / game::TileManager::TILE_SIZE);
  if (minY < 0) {
    minY = 0;
  }

  // Calculate number of tiles in camera
  u32 countX = std::ceil(camera.GetWidth() / game::TileManager::TILE_SIZE);
  if (countX + minX > terrain.GetWidth()) {
    countX = terrain.GetWidth() - minX;
  }
  u32 countY = std::ceil(camera.GetHeight() / game::TileManager::TILE_SIZE);
  if (countY + minY > terrain.GetHeight()) {
    countY = terrain.GetHeight() - minY;
  }

  // Render each tile
  for (u32 y = minY; y < minY + countY + 1; y++) {
    for (u32 x = minX; x < minX + countX + 1; x++) {
      Vector3F position =
        Vector3F(x * game::TileManager::TILE_SIZE - cameraPos.x,
                 y * game::TileManager::TILE_SIZE - cameraPos.y,
                 0.5f);
      alflib::Color tint = alflib::Color::WHITE;
      std::shared_ptr<game::Tile> tile =
        terrain.GetTile(x, y, Terrain::LAYER_FOREGROUND);
      const game::ResourcePath& resourcePath =
        tile->GetResourcePath(world, x, y);
      Vector2F texMin, texMax;
      tileAtlas.GetTextureCoordinates(resourcePath, texMin, texMax);
      spriteBatch.Submit(
        tileAtlas.GetTexture(),
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

}