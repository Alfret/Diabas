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
  delete mDataCells;
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
  for (u32 y = minY; y < minY + countY + 2; y++) {
    for (u32 x = minX; x < minX + countX + 2; x++) {
      WorldPos worldPosition{ x, y };
      Cell& cell = GetCell(worldPosition);
      alflib::Color tint = alflib::Color::WHITE;

      // Render tile
      Vector3F renderPosition = Vector3F(
        x * TILE_SIZE - cameraPos.x, y * TILE_SIZE - cameraPos.y, 0.0f);
      spriteBatch.Submit(mClientCache.GetTileAtlasTexture(),
                         renderPosition,
                         Vector2F(TILE_SIZE, TILE_SIZE),
                         tint,
                         cell.texMinTile,
                         cell.texMaxTile);
    }
  }

  // Render each wall
  // TODO(Filip Björklund): Implement zooming
  for (u32 y = minY; y < minY + countY + 2; y++) {
    for (u32 x = minX; x < minX + countX + 2; x++) {
      WorldPos worldPosition{ x, y };
      Cell& cell = GetCell(worldPosition);
      alflib::Color tint = alflib::Color::WHITE;

      // Render wall
      Vector3F renderPosWall =
        Vector3F(f32(x * WALL_SIZE) - cameraPos.x - (WALL_SIZE / 2.0f),
                 f32(y * WALL_SIZE) - cameraPos.y - (WALL_SIZE / 2.0f),
                 -0.5f);
      spriteBatch.Submit(mClientCache.GetWallAtlasTexture(),
                         renderPosWall,
                         Vector2F(WALL_SIZE * 2.0f, WALL_SIZE * 2.0f),
                         tint,
                         cell.texMinWall,
                         cell.texMaxWall);
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
  const Vector3F& cameraPos = camera.GetPosition();
  WorldPos pos{ u32((cameraPos.x + mousePosition.x) / TILE_SIZE),
                u32((cameraPos.y + mousePosition.y) / TILE_SIZE) };
  pos.X() = alflib::Clamp(pos.X(), 0u, mWorld.GetTerrain().GetWidth());
  pos.Y() = alflib::Clamp(pos.Y(), 0u, mWorld.GetTerrain().GetHeight());
  return pos;
}

// -------------------------------------------------------------------------- //

void
WorldRenderer::OnResize(u32 width, u32 height)
{
  delete mDataCells;
  mDataCells = new Cell[width * height];
}

// -------------------------------------------------------------------------- //

void
WorldRenderer::OnTileChanged(WorldPos pos)
{
  TileRegistry::TileID id = mWorld.GetTerrain().GetTileID(pos);
  Tile* tile = TileRegistry::Instance().GetTile(id);
  Cell& cell = GetCell(pos);

  mClientCache.GetTextureCoordinatesForTile(
    id, tile->GetResourceIndex(mWorld, pos), cell.texMinTile, cell.texMaxTile);
  std::swap(cell.texMinTile.x, cell.texMaxTile.x);
}

// -------------------------------------------------------------------------- //

void
WorldRenderer::OnWallChanged(WorldPos pos)
{
  WallRegistry::WallID id = mWorld.GetTerrain().GetWallID(pos);
  Wall* wall = WallRegistry::Instance().GetWall(id);
  Cell& cell = GetCell(pos);

  mClientCache.GetTextureCoordinatesForWall(
    id, wall->GetResourceIndex(mWorld, pos), cell.texMinWall, cell.texMaxWall);
  std::swap(cell.texMinWall.x, cell.texMaxWall.x);
}

// -------------------------------------------------------------------------- //

WorldRenderer::Cell&
WorldRenderer::GetCell(WorldPos pos)
{
  return *(mDataCells + mWorld.GetTerrain().GetWidth() * pos.Y() + pos.X());
}

}