#include "game/gameplay/tile/tile_variant.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/world.hpp"
#include "game/terrain.hpp"

// ========================================================================== //
// Private Functions
// ========================================================================== //

namespace dib::game {

u8
CalculateNeighbourMask(TileVariant* tile, Terrain& terrain, WorldPos pos)
{
  //  1 -  2 -   4
  //  8 -    -  16
  // 32 - 64 - 128

  u8 mask = 0;

  // Top
  if (terrain.IsValidPosition(pos.TopLeft()) &&
      terrain.GetTile(pos.TopLeft()) == tile) {
    mask |= TileVariant::kTopLeft;
  }
  if (terrain.IsValidPosition(pos.Top()) &&
      terrain.GetTile(pos.Top()) == tile) {
    mask |= TileVariant::kTop;
  }
  if (terrain.IsValidPosition(pos.TopRight()) &&
      terrain.GetTile(pos.TopRight()) == tile) {
    mask |= TileVariant::kTopRight;
  }

  // Center
  if (terrain.IsValidPosition(pos.Left()) &&
      terrain.GetTile(pos.Left()) == tile) {
    mask |= TileVariant::kLeft;
  }
  if (terrain.IsValidPosition(pos.Right()) &&
      terrain.GetTile(pos.Right()) == tile) {
    mask |= TileVariant::kRight;
  }

  // Bottom
  if (terrain.IsValidPosition(pos.BottomLeft()) &&
      terrain.GetTile(pos.BottomLeft()) == tile) {
    mask |= TileVariant::kBottomLeft;
  }
  if (terrain.IsValidPosition(pos.Bottom()) &&
      terrain.GetTile(pos.Bottom()) == tile) {
    mask |= TileVariant::kBottom;
  }
  if (terrain.IsValidPosition(pos.BottomRight()) &&
      terrain.GetTile(pos.BottomRight()) == tile) {
    mask |= TileVariant::kBottomRight;
  }

  return mask;
}

}

// ==========================================================================
// // TileVariant Implementation
// ==========================================================================
// //

namespace dib::game {

TileVariant::TileVariant(const ResourcePath& resourcePath,
                         const String& translationKey)
  : Tile(resourcePath, translationKey)
{}

// --------------------------------------------------------------------------
// //

u32
TileVariant::GetResourceIndex(World& world, WorldPos pos)
{
  //  1 -  2 -   4
  //  8 -    -  16
  // 32 - 64 - 128

  // A -> 0 * 13 = 0
  // B -> 1 * 13 = 13
  // C -> 2 * 13 = 26
  // D -> 3 * 13 = 39
  // E -> 4 * 13 = 52

  Terrain& terrain = world.GetTerrain();
  u8 mask = CalculateNeighbourMask(this, terrain, pos);
  u32 index = LookupResourceIndex(mask);
  return index;
}

// -------------------------------------------------------------------------- //

u32
TileVariant::LookupResourceIndex(u8 mask)
{
  static u32 resourceTable[255u];

  // 0.
  resourceTable[0] = 15;
  // 1.
  resourceTable[kTopLeft] = 15;
  // 2.
  resourceTable[kTop] = 20;
  // 3.
  resourceTable[kTopLeft | kTop] = 20;
  // 4.
  resourceTable[kTopRight] = 15;
  // 5.
  resourceTable[kTopLeft | kTopRight] = 15;
  // 6.
  resourceTable[kTop | kTopRight] = 20;
  // 7.
  resourceTable[kTopLeft | kTop | kTopRight] = 20;

  // ------------------------------------------------------------------------ //

  // 8.
  resourceTable[kLeft] = 33;
  // 9.
  resourceTable[kLeft | kTopLeft] = 33;
  // 10.
  resourceTable[kLeft | kTop] = 26;
  // 11.
  resourceTable[kLeft | kTopLeft | kTop] = 23;
  // 12.
  resourceTable[kLeft | kTopRight] = 33;
  // 13.
  resourceTable[kLeft | kTopLeft | kTopRight] = 33;
  // 14.
  resourceTable[kLeft | kTop | kTopRight] = 26;
  // 15.
  resourceTable[kLeft | kTopLeft | kTop | kTopRight] = 23;

  // ------------------------------------------------------------------------ //

  // 16.
  resourceTable[kRight] = 31;
  // 17.
  resourceTable[kRight | kTopLeft] = 31;
  // 18.
  resourceTable[kRight | kTop] = 24;
  // 19.
  resourceTable[kRight | kTopLeft | kTop] = 24;
  // 20.
  resourceTable[kRight | kTopRight] = 31;
  // 21.
  resourceTable[kRight | kTopLeft | kTopRight] = 31;
  // 22.
  resourceTable[kRight | kTop | kTopRight] = 21;
  // 23.
  resourceTable[kRight | kTopLeft | kTop | kTopRight] = 21;
  // 24.
  resourceTable[kRight | kLeft] = 32;
  // 25.
  resourceTable[kRight | kLeft | kTopLeft] = 32;
  // 26.
  resourceTable[kRight | kLeft | kTop] = 25;
  // 27.
  resourceTable[kRight | kLeft | kTopLeft | kTop] = 46;
  // 28.
  resourceTable[kRight | kLeft | kTopRight] = 32;
  // 29.
  resourceTable[kRight | kLeft | kTopLeft | kTopRight] = 32;
  // 30.
  resourceTable[kRight | kLeft | kTop | kTopRight] = 45;
  // 31.
  resourceTable[kRight | kLeft | kTopLeft | kTop | kTopRight] = 22;

  // ------------------------------------------------------------------------ //

  // 32.
  resourceTable[kBottomLeft] = 15;
  // 33.
  resourceTable[kBottomLeft | kTopLeft] = 15;
  // 34.
  resourceTable[kBottomLeft | kTop] = 20;
  // 35.
  resourceTable[kBottomLeft | kTopLeft | kTop] = 20;
  // 36.
  resourceTable[kBottomLeft | kTopRight] = 15;
  // 37.
  resourceTable[kBottomLeft | kTopLeft | kTopRight] = 15;
  // 38.
  resourceTable[kBottomLeft | kTop | kTopRight] = 20;
  // 39.
  resourceTable[kBottomLeft | kTopLeft | kTop | kTopRight] = 20;
  // 40.
  resourceTable[kBottomLeft | kLeft] = 33;
  // 41.
  resourceTable[kBottomLeft | kLeft | kTopLeft] = 33;
  // 42.
  resourceTable[kBottomLeft | kLeft | kTop] = 26;
  // 43.
  resourceTable[kBottomLeft | kLeft | kTopLeft | kTop] = 23;
  // 44.
  resourceTable[kBottomLeft | kLeft | kTopRight] = 33;
  // 45.
  resourceTable[kBottomLeft | kLeft | kTopLeft | kTopRight] = 33;
  // 46.
  resourceTable[kBottomLeft | kLeft | kTop | kTopRight] = 26;
  // 47.
  resourceTable[kBottomLeft | kLeft | kTopLeft | kTop | kTopRight] = 23;
  // 48.
  resourceTable[kBottomLeft | kRight] = 31;
  // 49.
  resourceTable[kBottomLeft | kRight | kTopLeft] = 31;
  // 50.
  resourceTable[kBottomLeft | kRight | kTop] = 24;
  // 51.
  resourceTable[kBottomLeft | kRight | kTopLeft | kTop] = 24;
  // 52.
  resourceTable[kBottomLeft | kRight | kTopRight] = 31;
  // 53.
  resourceTable[kBottomLeft | kRight | kTopLeft | kTopRight] = 31;
  // 54.
  resourceTable[kBottomLeft | kRight | kTop | kTopRight] = 21;
  // 55.
  resourceTable[kBottomLeft | kRight | kTopLeft | kTop | kTopRight] = 21;
  // 56.
  resourceTable[kBottomLeft | kRight | kLeft] = 32;
  // 57.
  resourceTable[kBottomLeft | kRight | kLeft | kTopLeft] = 32;
  // 58.
  resourceTable[kBottomLeft | kRight | kLeft | kTop] = 25;
  // 59.
  resourceTable[kBottomLeft | kRight | kLeft | kTopLeft | kTop] = 46;
  // 60.
  resourceTable[kBottomLeft | kRight | kLeft | kTopRight] = 32;
  // 61.
  resourceTable[kBottomLeft | kRight | kLeft | kTopLeft | kTopRight] = 32;
  // 62.
  resourceTable[kBottomLeft | kRight | kLeft | kTop | kTopRight] = 45;
  // 63.
  resourceTable[kBottomLeft | kRight | kLeft | kTopLeft | kTop | kTopRight] =
    22;

  // ------------------------------------------------------------------------ //

  // 64.
  resourceTable[kBottom] = 0;
  // 65.
  resourceTable[kBottom | kTopLeft] = 0;
  // 66.
  resourceTable[kBottom | kTop] = 10;
  // 67.
  resourceTable[kBottom | kTopLeft | kTop] = 10;
  // 68.
  resourceTable[kBottom | kTopRight] = 0;
  // 69.
  resourceTable[kBottom | kTopLeft | kTopRight] = 0;
  // 70.
  resourceTable[kBottom | kTop | kTopRight] = 10;
  // 71.
  resourceTable[kBottom | kTopLeft | kTop | kTopRight] = 10;
  // 72.
  resourceTable[kBottom | kLeft] = 6;
  // 73.
  resourceTable[kBottom | kLeft | kTopLeft] = 6;
  // 74.
  resourceTable[kBottom | kLeft | kTop] = 16;
  // 75.
  resourceTable[kBottom | kLeft | kTopLeft | kTop] = 35;
  // 76.
  resourceTable[kBottom | kLeft | kTopRight] = 6;
  // 77.
  resourceTable[kBottom | kLeft | kTopLeft | kTopRight] = 6;
  // 78.
  resourceTable[kBottom | kLeft | kTop | kTopRight] = 16;
  // 79.
  resourceTable[kBottom | kLeft | kTopLeft | kTop | kTopRight] = 35;
  // 80.
  resourceTable[kBottom | kRight] = 4;
  // 81.
  resourceTable[kBottom | kRight | kTopLeft] = 4;
  // 82.
  resourceTable[kBottom | kRight | kTop] = 14;
  // 83.
  resourceTable[kBottom | kRight | kTopLeft | kTop] = 14;
  // 84.
  resourceTable[kBottom | kRight | kTopRight] = 4;
  // 85.
  resourceTable[kBottom | kRight | kTopLeft | kTopRight] = 4;
  // 86.
  resourceTable[kBottom | kRight | kTop | kTopRight] = 36;
  // 87.
  resourceTable[kBottom | kRight | kTopLeft | kTop | kTopRight] = 36;
  // 88.
  resourceTable[kBottom | kRight | kLeft] = 5;
  // 89.
  resourceTable[kBottom | kRight | kLeft | kTopLeft] = 5;
  // 90.
  resourceTable[kBottom | kRight | kLeft | kTop] = 18;
  // 91.
  resourceTable[kBottom | kRight | kLeft | kTopLeft | kTop] = 38;
  // 92.
  resourceTable[kBottom | kRight | kLeft | kTopRight] = 5;
  // 93.
  resourceTable[kBottom | kRight | kLeft | kTopLeft | kTopRight] = 5;
  // 94.
  resourceTable[kBottom | kRight | kLeft | kTop | kTopRight] = 39;
  // 95.
  resourceTable[kBottom | kRight | kLeft | kTopLeft | kTop | kTopRight] = 8;
  // 96.
  resourceTable[kBottom | kBottomLeft] = 0;
  // 97.
  resourceTable[kBottom | kBottomLeft | kTopLeft] = 0;
  // 98.
  resourceTable[kBottom | kBottomLeft | kTop] = 10;
  // 99.
  resourceTable[kBottom | kBottomLeft | kTopLeft | kTop] = 10;
  // 100.
  resourceTable[kBottom | kBottomLeft | kTopRight] = 0;
  // 101.
  resourceTable[kBottom | kBottomLeft | kTopLeft | kTopRight] = 0;
  // 102.
  resourceTable[kBottom | kBottomLeft | kTop | kTopRight] = 10;
  // 103.
  resourceTable[kBottom | kBottomLeft | kTopLeft | kTop | kTopRight] = 10;
  // 104.
  resourceTable[kBottom | kBottomLeft | kLeft] = 3;
  // 105.
  resourceTable[kBottom | kBottomLeft | kLeft | kTopLeft] = 3;
  // 106.
  resourceTable[kBottom | kBottomLeft | kLeft | kTop] = 47;
  // 107.
  resourceTable[kBottom | kBottomLeft | kLeft | kTopLeft | kTop] = 13;
  // 108.
  resourceTable[kBottom | kBottomLeft | kLeft | kTopRight] = 3;
  // 109.
  resourceTable[kBottom | kBottomLeft | kLeft | kTopLeft | kTopRight] = 3;
  // 110.
  resourceTable[kBottom | kBottomLeft | kLeft | kTop | kTopRight] = 47;
  // 111.
  resourceTable[kBottom | kBottomLeft | kLeft | kTopLeft | kTop | kTopRight] =
    13;
  // 112.
  resourceTable[kBottom | kBottomLeft | kRight] = 4;
  // 113.
  resourceTable[kBottom | kBottomLeft | kRight | kTopLeft] = 4;
  // 114.
  resourceTable[kBottom | kBottomLeft | kRight | kTop] = 14;
  // 115.
  resourceTable[kBottom | kBottomLeft | kRight | kTopLeft | kTop] = 14;
  // 116.
  resourceTable[kBottom | kBottomLeft | kRight | kTopRight] = 4;
  // 117.
  resourceTable[kBottom | kBottomLeft | kRight | kTopLeft | kTopRight] = 4;
  // 118.
  resourceTable[kBottom | kBottomLeft | kRight | kTop | kTopRight] = 36;
  // 119.
  resourceTable[kBottom | kBottomLeft | kRight | kTopLeft | kTop | kTopRight] =
    36;
  // 120.
  resourceTable[kBottom | kBottomLeft | kRight | kLeft] = 34;
  // 121.
  resourceTable[kBottom | kBottomLeft | kRight | kLeft | kTopLeft] = 34;
  // 122.
  resourceTable[kBottom | kBottomLeft | kRight | kLeft | kTop] = 48;
  // 123.
  resourceTable[kBottom | kBottomLeft | kRight | kLeft | kTopLeft | kTop] = 17;
  // 124.
  resourceTable[kBottom | kBottomLeft | kRight | kLeft | kTopRight] = 34;
  // 125.
  resourceTable[kBottom | kBottomLeft | kRight | kLeft | kTopLeft | kTopRight] =
    34;
  // 126.
  resourceTable[kBottom | kBottomLeft | kRight | kLeft | kTop | kTopRight] = 43;
  // 127.
  resourceTable[kBottom | kBottomLeft | kRight | kLeft | kTopLeft | kTop |
                kTopRight] = 7;

  // ------------------------------------------------------------------------ //

  // 128.
  resourceTable[kBottomRight] = 15;
  // 129.
  resourceTable[kBottomRight | kTopLeft] = 15;
  // 130.
  resourceTable[kBottomRight | kTop] = 20;
  // 131.
  resourceTable[kBottomRight | kTopLeft | kTop] = 20;
  // 132.
  resourceTable[kBottomRight | kTopRight] = 15;
  // 133.
  resourceTable[kBottomRight | kTopLeft | kTopRight] = 15;
  // 134.
  resourceTable[kBottomRight | kTop | kTopRight] = 20;
  // 135.
  resourceTable[kBottomRight | kTopLeft | kTop | kTopRight] = 20;
  // 136.
  resourceTable[kBottomRight | kLeft] = 33;
  // 137.
  resourceTable[kBottomRight | kLeft | kTopLeft] = 33;
  // 138.
  resourceTable[kBottomRight | kLeft | kTop] = 26;
  // 139.
  resourceTable[kBottomRight | kLeft | kTopLeft | kTop] = 23;
  // 140.
  resourceTable[kBottomRight | kLeft | kTopRight] = 33;
  // 141.
  resourceTable[kBottomRight | kLeft | kTopLeft | kTopRight] = 33;
  // 142.
  resourceTable[kBottomRight | kLeft | kTop | kTopRight] = 26;
  // 143.
  resourceTable[kBottomRight | kLeft | kTopLeft | kTop | kTopRight] = 23;
  // 144.
  resourceTable[kBottomRight | kRight] = 31;
  // 145.
  resourceTable[kBottomRight | kRight | kTopLeft] = 31;
  // 146.
  resourceTable[kBottomRight | kRight | kTop] = 24;
  // 147.
  resourceTable[kBottomRight | kRight | kTopLeft | kTop] = 24;
  // 148.
  resourceTable[kBottomRight | kRight | kTopRight] = 31;
  // 149.
  resourceTable[kBottomRight | kRight | kTopLeft | kTopRight] = 31;
  // 150.
  resourceTable[kBottomRight | kRight | kTop | kTopRight] = 21;
  // 151.
  resourceTable[kBottomRight | kRight | kTopLeft | kTop | kTopRight] = 21;
  // 152.
  resourceTable[kBottomRight | kRight | kLeft] = 32;
  // 153.
  resourceTable[kBottomRight | kRight | kLeft | kTopLeft] = 32;
  // 154.
  resourceTable[kBottomRight | kRight | kLeft | kTop] = 25;
  // 155.
  resourceTable[kBottomRight | kRight | kLeft | kTopLeft | kTop] = 46;
  // 156.
  resourceTable[kBottomRight | kRight | kLeft | kTopRight] = 32;
  // 157.
  resourceTable[kBottomRight | kRight | kLeft | kTopLeft | kTopRight] = 32;
  // 158.
  resourceTable[kBottomRight | kRight | kLeft | kTop | kTopRight] = 45;
  // 159.
  resourceTable[kBottomRight | kRight | kLeft | kTopLeft | kTop | kTopRight] =
    22;
  // 160.
  resourceTable[kBottomRight | kBottomLeft] = 15;
  // 161.
  resourceTable[kBottomRight | kBottomLeft | kTopLeft] = 15;
  // 162.
  resourceTable[kBottomRight | kBottomLeft | kTop] = 20;
  // 163.
  resourceTable[kBottomRight | kBottomLeft | kTopLeft | kTop] = 20;
  // 164.
  resourceTable[kBottomRight | kBottomLeft | kTopRight] = 15;
  // 165.
  resourceTable[kBottomRight | kBottomLeft | kTopLeft | kTopRight] = 15;
  // 166.
  resourceTable[kBottomRight | kBottomLeft | kTop | kTopRight] = 20;
  // 167.
  resourceTable[kBottomRight | kBottomLeft | kTopLeft | kTop | kTopRight] = 20;
  // 168.
  resourceTable[kBottomRight | kBottomLeft | kLeft] = 33;
  // 169.
  resourceTable[kBottomRight | kBottomLeft | kLeft | kTopLeft] = 33;
  // 170.
  resourceTable[kBottomRight | kBottomLeft | kLeft | kTop] = 26;
  // 171.
  resourceTable[kBottomRight | kBottomLeft | kLeft | kTopLeft | kTop] = 23;
  // 172.
  resourceTable[kBottomRight | kBottomLeft | kLeft | kTopRight] = 33;
  // 173.
  resourceTable[kBottomRight | kBottomLeft | kLeft | kTopLeft | kTopRight] = 33;
  // 174.
  resourceTable[kBottomRight | kBottomLeft | kLeft | kTop | kTopRight] = 26;
  // 175.
  resourceTable[kBottomRight | kBottomLeft | kLeft | kTopLeft | kTop |
                kTopRight] = 23;
  // 176.
  resourceTable[kBottomRight | kBottomLeft | kRight] = 31;
  // 177.
  resourceTable[kBottomRight | kBottomLeft | kRight | kTopLeft] = 31;
  // 178.
  resourceTable[kBottomRight | kBottomLeft | kRight | kTop] = 24;
  // 179.
  resourceTable[kBottomRight | kBottomLeft | kRight | kTopLeft | kTop] = 24;
  // 180.
  resourceTable[kBottomRight | kBottomLeft | kRight | kTopRight] = 31;
  // 181.
  resourceTable[kBottomRight | kBottomLeft | kRight | kTopLeft | kTopRight] =
    31;
  // 182.
  resourceTable[kBottomRight | kBottomLeft | kRight | kTop | kTopRight] = 21;
  // 183.
  resourceTable[kBottomRight | kBottomLeft | kRight | kTopLeft | kTop |
                kTopRight] = 21;
  // 184.
  resourceTable[kBottomRight | kBottomLeft | kRight | kLeft] = 32;
  // 185.
  resourceTable[kBottomRight | kBottomLeft | kRight | kLeft | kTopLeft] = 32;
  // 186.
  resourceTable[kBottomRight | kBottomLeft | kRight | kLeft | kTop] = 25;
  // 187.
  resourceTable[kBottomRight | kBottomLeft | kRight | kLeft | kTopLeft | kTop] =
    46;
  // 188.
  resourceTable[kBottomRight | kBottomLeft | kRight | kLeft | kTopRight] = 32;
  // 189.
  resourceTable[kBottomRight | kBottomLeft | kRight | kLeft | kTopLeft |
                kTopRight] = 32;
  // 190.
  resourceTable[kBottomRight | kBottomLeft | kRight | kLeft | kTop |
                kTopRight] = 45;
  // 191.
  resourceTable[kBottomRight | kBottomLeft | kRight | kLeft | kTopLeft | kTop |
                kTopRight] = 22;
  // 192.
  resourceTable[kBottomRight | kBottom] = 0;
  // 193.
  resourceTable[kBottomRight | kBottom | kTopLeft] = 0;
  // 194.
  resourceTable[kBottomRight | kBottom | kTop] = 10;
  // 195.
  resourceTable[kBottomRight | kBottom | kTopLeft | kTop] = 10;
  // 196.
  resourceTable[kBottomRight | kBottom | kTopRight] = 0;
  // 197.
  resourceTable[kBottomRight | kBottom | kTopLeft | kTopRight] = 0;
  // 198.
  resourceTable[kBottomRight | kBottom | kTop | kTopRight] = 10;
  // 199.
  resourceTable[kBottomRight | kBottom | kTopLeft | kTop | kTopRight] = 10;
  // 200.
  resourceTable[kBottomRight | kBottom | kLeft] = 6;
  // 201.
  resourceTable[kBottomRight | kBottom | kLeft | kTopLeft] = 6;
  // 202.
  resourceTable[kBottomRight | kBottom | kLeft | kTop] = 16;
  // 203.
  resourceTable[kBottomRight | kBottom | kLeft | kTopLeft | kTop] = 35;
  // 204.
  resourceTable[kBottomRight | kBottom | kLeft | kTopRight] = 6;
  // 205.
  resourceTable[kBottomRight | kBottom | kLeft | kTopLeft | kTopRight] = 6;
  // 206.
  resourceTable[kBottomRight | kBottom | kLeft | kTop | kTopRight] = 16;
  // 207.
  resourceTable[kBottomRight | kBottom | kLeft | kTopLeft | kTop | kTopRight] =
    35;
  // 208.
  resourceTable[kBottomRight | kBottom | kRight] = 1;
  // 209.
  resourceTable[kBottomRight | kBottom | kRight | kTopLeft] = 1;
  // 210.
  resourceTable[kBottomRight | kBottom | kRight | kTop] = 44;
  // 211.
  resourceTable[kBottomRight | kBottom | kRight | kTopLeft | kTop] = 44;
  // 212.
  resourceTable[kBottomRight | kBottom | kRight | kTopRight] = 1;
  // 213.
  resourceTable[kBottomRight | kBottom | kRight | kTopLeft | kTopRight] = 1;
  // 214.
  resourceTable[kBottomRight | kBottom | kRight | kTop | kTopRight] = 11;
  // 215.
  resourceTable[kBottomRight | kBottom | kRight | kTopLeft | kTop | kTopRight] =
    11;
  // 216.
  resourceTable[kBottomRight | kBottom | kRight | kLeft] = 37;
  // 217.
  resourceTable[kBottomRight | kBottom | kRight | kLeft | kTopLeft] = 37;
  // 218.
  resourceTable[kBottomRight | kBottom | kRight | kLeft | kTop] = 49;
  // 219.
  resourceTable[kBottomRight | kBottom | kRight | kLeft | kTopLeft | kTop] = 42;
  // 220.
  resourceTable[kBottomRight | kBottom | kRight | kLeft | kTopRight] = 37;
  // 221.
  resourceTable[kBottomRight | kBottom | kRight | kLeft | kTopLeft |
                kTopRight] = 37;
  // 222.
  resourceTable[kBottomRight | kBottom | kRight | kLeft | kTop | kTopRight] =
    19;
  // 223.
  resourceTable[kBottomRight | kBottom | kRight | kLeft | kTopLeft | kTop |
                kTopRight] = 9;
  // 224.
  resourceTable[kBottomRight | kBottom | kBottomLeft] = 0;
  // 225.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kTopLeft] = 0;
  // 226.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kTop] = 10;
  // 227.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kTopLeft | kTop] = 10;
  // 228.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kTopRight] = 0;
  // 229.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kTopLeft | kTopRight] =
    0;
  // 230.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kTop | kTopRight] = 10;
  // 231.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kTopLeft | kTop |
                kTopRight] = 10;
  // 232.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kLeft] = 3;
  // 233.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kLeft | kTopLeft] = 3;
  // 234.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kLeft | kTop] = 47;
  // 235.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kLeft | kTopLeft |
                kTop] = 13;
  // 236.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kLeft | kTopRight] = 3;
  // 237.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kLeft | kTopLeft |
                kTopRight] = 3;
  // 238.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kLeft | kTop |
                kTopRight] = 47;
  // 239.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kLeft | kTopLeft | kTop |
                kTopRight] = 13;
  // 240.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight] = 1;
  // 241.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kTopLeft] = 1;
  // 242.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kTop] = 44;
  // 243.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kTopLeft |
                kTop] = 44;
  // 244.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kTopRight] = 1;
  // 245.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kTopLeft |
                kTopRight] = 1;
  // 246.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kTop |
                kTopRight] = 11;
  // 247.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kTopLeft |
                kTop | kTopRight] = 11;
  // 248.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kLeft] = 2;
  // 249.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kLeft |
                kTopLeft] = 2;
  // 250.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kLeft | kTop] =
    28;
  // 251.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kLeft |
                kTopLeft | kTop] = 27;
  // 252.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kLeft |
                kTopRight] = 2;
  // 253.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kLeft |
                kTopLeft | kTopRight] = 2;
  // 254.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kLeft | kTop |
                kTopRight] = 29;
  // 255.
  resourceTable[kBottomRight | kBottom | kBottomLeft | kRight | kLeft |
                kTopLeft | kTop | kTopRight] = 12;

  return resourceTable[mask];
}

}