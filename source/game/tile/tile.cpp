#include "game/tile/tile.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/tile/tile_entity.hpp"

// ========================================================================== //
// Tile Implementation
// ========================================================================== //

namespace dib::game {

Tile::Tile(ResourcePath resourcePath, String translationKey)
  : mResourcePath(std::move(resourcePath))
  , mTranslationKey(std::move(translationKey))
{}

// -------------------------------------------------------------------------- //

void
Tile::OnPlaced([[maybe_unused]] World& world, [[maybe_unused]] WorldPos pos)
{}

// -------------------------------------------------------------------------- //

void
Tile::OnDestroyed([[maybe_unused]] World& world, [[maybe_unused]] WorldPos pos)
{}

// -------------------------------------------------------------------------- //

bool
Tile::OnActivated([[maybe_unused]] World& world, [[maybe_unused]] WorldPos pos)
{
  return false;
}

// -------------------------------------------------------------------------- //

void
Tile::OnNeighbourChange([[maybe_unused]] World& world,
                        [[maybe_unused]] WorldPos pos)
{}

// -------------------------------------------------------------------------- //

bool
Tile::IsMultiTile([[maybe_unused]] World& world, [[maybe_unused]] WorldPos pos)
{
  return false;
}

// -------------------------------------------------------------------------- //

bool
Tile::CanPlaceMultiTile([[maybe_unused]] World& world,
                        [[maybe_unused]] WorldPos pos)
{
  return false;
}

// -------------------------------------------------------------------------- //

bool
Tile::PlaceMultiTile([[maybe_unused]] World& world,
                     [[maybe_unused]] WorldPos pos)
{
  return false;
}

// -------------------------------------------------------------------------- //

bool
Tile::KillMultiTile([[maybe_unused]] World& world,
                    [[maybe_unused]] WorldPos pos)
{
  return false;
}

// -------------------------------------------------------------------------- //

bool
Tile::IsLightEmitter([[maybe_unused]] World& world,
                     [[maybe_unused]] WorldPos pos)
{
  return mIsLightEmitter;
}

// -------------------------------------------------------------------------- //

Tile*
Tile::SetIsLightEmitter(bool isLightEmitter)
{
  mIsLightEmitter = isLightEmitter;
  return this;
}

// -------------------------------------------------------------------------- //

f32
Tile::GetLightStrength([[maybe_unused]] World& world,
                       [[maybe_unused]] WorldPos pos)
{
  return mLightStrength;
}

// -------------------------------------------------------------------------- //

Tile*
Tile::SetLightStrength(f32 lightStrength)
{
  mLightStrength = lightStrength;
  return this;
}

// -------------------------------------------------------------------------- //

Color
Tile::GetLightColor([[maybe_unused]] World& world,
                    [[maybe_unused]] WorldPos pos)
{
  return mLightColor;
}

// -------------------------------------------------------------------------- //

Tile*
Tile::SetLightColor(Color lightColor)
{
  mLightColor = lightColor;
  return this;
}

// -------------------------------------------------------------------------- //

f32
Tile::GetOpacity([[maybe_unused]] World& world, [[maybe_unused]] WorldPos pos)
{
  return mOpacity;
}

// -------------------------------------------------------------------------- //

Tile*
Tile::SetOpacity(f32 opacity)
{
  mOpacity = opacity;
  return this;
}

// -------------------------------------------------------------------------- //

CollisionType
Tile::GetCollision([[maybe_unused]] const World& world,
                   [[maybe_unused]] WorldPos pos) const
{
  return mCollisionType;
}

// -------------------------------------------------------------------------- //

Tile*
Tile::SetCollisionType(CollisionType collisionType)
{
  mCollisionType = collisionType;
  return this;
}

// -------------------------------------------------------------------------- //

f32
Tile::GetHardness([[maybe_unused]] World& world, [[maybe_unused]] WorldPos pos)
{
  return mHardness;
}

// -------------------------------------------------------------------------- //

Tile*
Tile::SetHardness(f32 hardness)
{
  mHardness = hardness;
  return this;
}

// -------------------------------------------------------------------------- //

u32
Tile::GetMinimumToolLevel()
{
  return mMinimumToolLevel;
}

// -------------------------------------------------------------------------- //

Tile*
Tile::SetMinimumToolLevel(u32 toolLevel)
{
  mMinimumToolLevel = toolLevel;
  return this;
}

// -------------------------------------------------------------------------- //

ToolType
Tile::GetRequiredToolType()
{
  return mRequiredToolType;
}

// -------------------------------------------------------------------------- //

Tile*
Tile::SetRequiredToolType(ToolType toolType)
{
  mRequiredToolType = toolType;
  return this;
}

// -------------------------------------------------------------------------- //

bool
Tile::IsDestructible()
{
  return mIsDestructible;
}

// -------------------------------------------------------------------------- //

Tile*
Tile::SetIsDestructible(bool isDestructible)
{
  mIsDestructible = isDestructible;
  return this;
}

// -------------------------------------------------------------------------- //

bool
Tile::CanBeReplaced([[maybe_unused]] World& world,
                    [[maybe_unused]] WorldPos pos)
{
  return mCanBeReplaced;
}

// -------------------------------------------------------------------------- //

Tile*
Tile::SetCanBeReplaced(bool canBeReplaced)
{
  mCanBeReplaced = canBeReplaced;
  return this;
}

// -------------------------------------------------------------------------- //

bool
Tile::HasTileEntity([[maybe_unused]] World& world,
                    [[maybe_unused]] WorldPos pos)
{
  return false;
}

// -------------------------------------------------------------------------- //

std::unique_ptr<TileEntity>
Tile::CreateTileEntity([[maybe_unused]] World& world,
                       [[maybe_unused]] WorldPos pos)
{
  return std::unique_ptr<TileEntity>(nullptr);
}

// -------------------------------------------------------------------------- //

u32
Tile::GetResourceIndex([[maybe_unused]] World& world,
                       [[maybe_unused]] WorldPos pos)
{
  return 0;
}

// -------------------------------------------------------------------------- //

const String&
Tile::GetTranslationKey([[maybe_unused]] World& world,
                        [[maybe_unused]] WorldPos pos) const
{
  return mTranslationKey;
}

// -------------------------------------------------------------------------- //

const String&
Tile::GetTranslationKey() const
{
  return mTranslationKey;
}

// -------------------------------------------------------------------------- //

const ResourcePath&
Tile::GetResourcePath() const
{
  return mResourcePath;
}

}
