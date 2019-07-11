#include "game/wall/wall.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/world.hpp"
#include "game/terrain.hpp"
#include "game/gameplay/core_content.hpp"

// ========================================================================== //
// Private Functions
// ========================================================================== //

namespace dib::game {

u8
CalculateNeighbourMask(Terrain& terrain, WorldPos pos)
{
  // - 1 -
  // 2 - 4
  // - 8 -

  u8 mask = 0;

  if (terrain.IsValidPosition(pos.Top()) &&
      terrain.GetWall(pos.Top()) != CoreContent::GetWalls().air) {
    mask |= Wall::kTop;
  }
  if (terrain.IsValidPosition(pos.Left()) &&
      terrain.GetWall(pos.Left()) != CoreContent::GetWalls().air) {
    mask |= Wall::kLeft;
  }
  if (terrain.IsValidPosition(pos.Right()) &&
      terrain.GetWall(pos.Right()) != CoreContent::GetWalls().air) {
    mask |= Wall::kRight;
  }
  if (terrain.IsValidPosition(pos.Bottom()) &&
      terrain.GetWall(pos.Bottom()) != CoreContent::GetWalls().air) {
    mask |= Wall::kBottom;
  }

  return mask;
}

}

// ========================================================================== //
// Wall Implementation
// ========================================================================== //

namespace dib::game {

Wall::Wall(ResourcePath resourcePath, String translationKey)
  : mResourcePath(std::move(resourcePath))
  , mTranslationKey(std::move(translationKey))
{}

// -------------------------------------------------------------------------- //

void
Wall::OnPlaced(World& world, WorldPos pos)
{}

// -------------------------------------------------------------------------- //

void
Wall::OnDestroyed(World& world, WorldPos pos)
{}

// -------------------------------------------------------------------------- //

void
Wall::OnNeighbourChange(World& world, WorldPos pos)
{}

// -------------------------------------------------------------------------- //

bool
Wall::IsDestructible()
{
  return mIsDestructible;
}

// -------------------------------------------------------------------------- //

Wall*
Wall::SetIsDestructible(bool isDestructible)
{
  mIsDestructible = isDestructible;
  return this;
}

// -------------------------------------------------------------------------- //

bool
Wall::CanBeReplaced([[maybe_unused]] World& world,
                    [[maybe_unused]] WorldPos pos)
{
  return mCanBeReplaced;
}

// -------------------------------------------------------------------------- //

Wall*
Wall::SetCanBeReplaced(bool canBeReplaced)
{
  mCanBeReplaced = canBeReplaced;
  return this;
}

// -------------------------------------------------------------------------- //

u32
Wall::GetResourceIndex([[maybe_unused]] World& world,
                       [[maybe_unused]] WorldPos pos)
{
  u8 mask = CalculateNeighbourMask(world.GetTerrain(), pos);
  static u8 resourceTable[16]; // = { 15, 13, 12, 8, 11, 7, 5, 3,
                               //    14, 6,  10, 2, 9,  1, 4, 0 };

  // 0.
  resourceTable[0] = 15;
  // 1.
  resourceTable[kTop] = 13;
  // 2.
  resourceTable[kLeft] = 12;
  // 3.
  resourceTable[kTop | kLeft] = 8;
  // 4.
  resourceTable[kRight] = 11;
  // 5.
  resourceTable[kTop | kRight] = 7;
  // 6.
  resourceTable[kLeft | kRight] = 5;
  // 7.
  resourceTable[kTop | kLeft | kRight] = 3;
  // 8.
  resourceTable[kBottom] = 14;
  // 9.
  resourceTable[kTop | kBottom] = 6;
  // 10.
  resourceTable[kLeft | kBottom] = 10;
  // 11.
  resourceTable[kTop | kLeft | kBottom] = 2;
  // 12.
  resourceTable[kRight | kBottom] = 9;
  // 13.
  resourceTable[kTop | kRight | kBottom] = 1;
  // 14.
  resourceTable[kLeft | kRight | kBottom] = 4;
  // 15.
  resourceTable[kTop | kLeft | kRight | kBottom] = 0;

  return resourceTable[mask];
}

// -------------------------------------------------------------------------- //

const String&
Wall::GetTranslationKey([[maybe_unused]] World& world,
                        [[maybe_unused]] WorldPos pos) const
{
  return mTranslationKey;
}

// -------------------------------------------------------------------------- //

const String&
Wall::GetTranslationKey() const
{
  return mTranslationKey;
}

// -------------------------------------------------------------------------- //

const ResourcePath&
Wall::GetResourcePath() const
{
  return mResourcePath;
}

}