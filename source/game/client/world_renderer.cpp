#include "game/client/world_renderer.hpp"

// ========================================================================== //
//
// ========================================================================== //

#include <microprofile/microprofile.h>

#include "graphics/renderer.hpp"
#include "graphics/camera.hpp"
#include "game/client/game_client.hpp"
#include "game/constants.hpp"

// ========================================================================== //
// TerrainRenderer Implementation
// ========================================================================== //

namespace dib::game {

WorldRenderer::WorldRenderer(World& world, ClientCache& clientCache)
  : mWorld(world)
  , mClientCache(clientCache)
{
  mWorld.GetTerrain().RegisterChangeListener(this);
  mDataCells =
    new Cell[mWorld.GetTerrain().GetWidth() * mWorld.GetTerrain().GetHeight()];
}

// -------------------------------------------------------------------------- //

WorldRenderer::~WorldRenderer()
{
  mWorld.GetTerrain().UnregisterChangeListener(this);
}

// -------------------------------------------------------------------------- //

void
WorldRenderer::Render(graphics::Renderer& renderer,
                      const graphics::Camera& camera)
{
  MICROPROFILE_SCOPEI("WorldRenderer", "RenderTerrain", MP_SANDYBROWN);

  // Retrieve objects
  Terrain& terrain = mWorld.GetTerrain();

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

      Vector3F renderPosition = Vector3F(
        x * TILE_SIZE - cameraPos.x, y * TILE_SIZE - cameraPos.y, 0.5f);
      alflib::Color tint = alflib::Color::WHITE;
      Cell& cell = GetCell(worldPosition);
      spriteBatch.Submit(mClientCache.GetTileAtlasTexture(),
                         renderPosition,
                         Vector2F(TILE_SIZE, TILE_SIZE),
                         tint,
                         cell.texMin,
                         cell.texMax);
    }
  }

  // Done rendering
  spriteBatch.End();
}

// -------------------------------------------------------------------------- //

WorldPos
WorldRenderer::PickScreenTile(const graphics::Camera& camera,
                              Vector2F mousePosition)
{
  // TODO(Filip Björklund): Implement support for zoom
  WorldPos pos{ u32(mousePosition.x / TILE_SIZE),
                u32(mousePosition.y / TILE_SIZE) };
  pos.X() = alflib::Clamp(pos.X(), 0u, mWorld.GetTerrain().GetWidth());
  pos.Y() = alflib::Clamp(pos.Y(), 0u, mWorld.GetTerrain().GetHeight());
  return pos;
}

// -------------------------------------------------------------------------- //

void
WorldRenderer::OnTileChanged(WorldPos pos)
{
  TileRegistry::TileID id = mWorld.GetTerrain().GetTileID(pos);
  Tile* tile = TileRegistry::Instance().GetTile(id);
  Cell& cell = GetCell(pos);

  mClientCache.GetTextureCoordinatesForTile(
    id, tile->GetResourceIndex(mWorld, pos), cell.texMin, cell.texMax);
  std::swap(cell.texMin.x, cell.texMax.x);
}

// -------------------------------------------------------------------------- //

void
WorldRenderer::OnWallChanged(WorldPos pos)
{}

// -------------------------------------------------------------------------- //

WorldRenderer::Cell&
WorldRenderer::GetCell(WorldPos pos)
{
  return *(mDataCells + mWorld.GetTerrain().GetWidth() * pos.Y() + pos.X());
}

}