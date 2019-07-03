#include "game/client/world_renderer.hpp"

// ========================================================================== //
//
// ========================================================================== //

#include "graphics/renderer.hpp"
#include "graphics/camera.hpp"
#include "game/client/game_client.hpp"
#include "game/constants.hpp"

// ========================================================================== //
// TerrainRenderer Implementation
// ========================================================================== //

namespace dib::game {

void
RenderWorldTerrain(graphics::Renderer& renderer,
                   const graphics::Camera& camera,
                   GameClient& gameClient)
{
  // Retrieve objects
  ClientCache& cache = gameClient.GetCache();
  TileRegistry& tileRegistry = gameClient.GetTileRegistry();
  World& world = gameClient.GetWorld();
  Terrain& terrain = world.GetTerrain();

  // Retrieve batch and begin
  graphics::SpriteBatch& spriteBatch = renderer.GetSpriteBatch();
  spriteBatch.Begin(&camera);

  // Calculate first tile in camera
  const Vector3F& cameraPos = camera.GetPosition();
  u32 minX = std::floor(camera.GetPosition().x / TILE_SIZE);
  if (minX < 0) {
    minX = 0;
  }
  u32 minY = std::floor(camera.GetPosition().y / TILE_SIZE);
  if (minY < 0) {
    minY = 0;
  }

  // Calculate number of tiles in camera
  u32 countX = std::ceil(camera.GetWidth() / TILE_SIZE);
  if (countX + minX > terrain.GetWidth()) {
    countX = terrain.GetWidth() - minX;
  }
  u32 countY = std::ceil(camera.GetHeight() / TILE_SIZE);
  if (countY + minY > terrain.GetHeight()) {
    countY = terrain.GetHeight() - minY;
  }

  // Render each tile
  // TODO(Filip Björklund): Implement zooming
  for (u32 y = minY; y < minY + countY + 2; y++) {
    for (u32 x = minX; x < minX + countX + 2; x++) {

      WorldPos worldPosition{ x, y };
      TileRegistry::TileID tileId = terrain.GetTileID(worldPosition);
      Tile* tile = tileRegistry.GetTile(tileId);
      u32 resourceIndex = tile->GetResourceIndex(world, worldPosition);

      Vector3F renderPosition = Vector3F(
        x * TILE_SIZE - cameraPos.x, y * TILE_SIZE - cameraPos.y, 0.5f);
      alflib::Color tint = alflib::Color::WHITE;
      Vector2F texMin, texMax;
      cache.GetTextureCoordinatesForTile(tileId, resourceIndex, texMin, texMax);
      spriteBatch.Submit(cache.GetTileAtlasTexture(),
                         renderPosition,
                         Vector2F(TILE_SIZE, TILE_SIZE),
                         tint,
                         texMin,
                         texMax);
    }
  }

  // Done rendering
  spriteBatch.End();
}

// -------------------------------------------------------------------------- //

WorldPos
PickWorldPosition(const World& world,
                  const graphics::Camera& camera,
                  Vector2F mousePos)
{
  // TODO(Filip Björklund): Implement support for zoom
  WorldPos pos{ u32(mousePos.x / TILE_SIZE), u32(mousePos.y / TILE_SIZE) };
  pos.X() = alflib::Clamp(pos.X(), 0u, world.GetTerrain().GetWidth());
  pos.Y() = alflib::Clamp(pos.Y(), 0u, world.GetTerrain().GetHeight());
  return pos;
}

}