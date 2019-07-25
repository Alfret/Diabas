#include "world.hpp"
#include "game/gameplay/moveable.hpp"
#include "game/gameplay/soul.hpp"
#include <dutil/stopwatch.hpp>
#include <microprofile/microprofile.h>

// ========================================================================== //
// Headers
// ========================================================================== //

#include <alflib/file/file_io.hpp>
#include <dutil/stopwatch.hpp>

#include "game/item/item_registry.hpp"

// ========================================================================== //
// World Implementation
// ========================================================================== //

namespace dib::game {

World::World()
  : mTerrain(this, Terrain::Size::kTiny)
{}

// -------------------------------------------------------------------------- //

World::World(World&& other)
  : mTerrain(std::move(other.mTerrain))
  , entity_manager_(std::move(other.entity_manager_))
  , network_(std::move(other.network_))
  , chat_(std::move(other.chat_))
{}

// -------------------------------------------------------------------------- //

World&
World::operator=(World&& other)
{
  if (this != &other) {
    mTerrain = std::move(other.mTerrain);
    entity_manager_ = std::move(other.entity_manager_);
    network_ = std::move(other.network_);
    chat_ = std::move(other.chat_);
  }
  return *this;
}

// -------------------------------------------------------------------------- //

void
World::Update(const f64 delta)
{
  MICROPROFILE_SCOPEI("world", "update", MP_BLUE);
  network_.Update();
  UpdateMoveables(*this, delta);
  UpdateSoul(*this, delta);

  // if constexpr(kSide == Side::kServer) {
  UpdateNpcs(*this, delta);
  //    }
}

// -------------------------------------------------------------------------- //

void
World::OnDisconnect()
{
  GetNpcRegistry().Clear();
  GetEntityManager().Clear();
}

// -------------------------------------------------------------------------- //

void
World::OnCommandNetwork(const std::string_view input)
{
  network_.NetworkInfo(input);
}

// -------------------------------------------------------------------------- //

void
World::OnCommandBroadcast(const std::string_view input)
{
  network_.Broadcast(input);
}

// -------------------------------------------------------------------------- //

bool
World::Save(const Path& path, bool overwrite)
{
  // Serialize world
  alflib::Buffer wb(10);
  alflib::MemoryWriter w(wb);
  w.Write(*this);

  // Open file
  alflib::FileIO::Flag flags =
    alflib::FileIO::Flag::kWrite | alflib::FileIO::Flag::kCreate;
  if (overwrite) {
    flags |= alflib::FileIO::Flag::kOverwrite;
  }
  alflib::FileIO io(path);
  alflib::FileResult result = io.Open(flags);
  if (result != alflib::FileResult::kSuccess) {
    DLOG_WARNING("Failed to open world file for saving");
    return false;
  }

  // Write world to file
  u64 written;
  result = io.Write(wb.GetData(), wb.GetSize(), written);
  if (result != alflib::FileResult::kSuccess) {
    DLOG_WARNING("Failed to write world to file");
    return false;
  }

  return true;
}

// -------------------------------------------------------------------------- //

bool
World::Load(const Path& path)
{
  dutil::Stopwatch sw;
  sw.Start();

  // Open file
  alflib::FileIO::Flag flags = alflib::FileIO::Flag::kRead;
  alflib::FileIO io(path);
  alflib::FileResult result = io.Open(flags);
  if (result != alflib::FileResult::kSuccess) {
    DLOG_WARNING("Failed to open world file ({}) for loading",
                 path.GetPathString());
    return false;
  }

  // Read world data
  u64 size = io.GetFile().GetSize();
  alflib::Buffer buffer(size);
  u64 read;
  result = io.Read(buffer.GetData(), size, read);
  if (result != alflib::FileResult::kSuccess) {
    DLOG_WARNING("Failed to read world file ({})", path.GetPathString());
    return false;
  }

  alflib::MemoryReader reader(buffer);
  bool success = Load(reader);

  sw.Stop();
  f64 time = sw.fs();
  DLOG_VERBOSE("World loading completed in {:.3f}s", time);

  return success;
}

// -------------------------------------------------------------------------- //

bool
World::Load(alflib::MemoryReader& reader)
{
  // Read tile table
  {
    u32 tileCount = reader.Read<u32>();
    tsl::robin_map<String, TileRegistry::TileID> tileMap;
    for (u32 i = 0; i < tileCount; i++) {
      String key = reader.Read<String>();
      TileRegistry::TileID id = reader.Read<TileRegistry::TileID>();
      tileMap[key] = id;
    }
  }

  // Read wall table
  {
    u32 wallCount = reader.Read<u32>();
    tsl::robin_map<String, WallRegistry::WallID> wallMap;
    for (u32 i = 0; i < wallCount; i++) {
      String key = reader.Read<String>();
      WallRegistry::WallID id = reader.Read<WallRegistry::WallID>();
      wallMap[key] = id;
    }
  }

  // Read item table
  {
    u32 itemCount = reader.Read<u32>();
    tsl::robin_map<String, ItemRegistry::ItemID> itemMap;
    for (u32 i = 0; i < itemCount; i++) {
      String key = reader.Read<String>();
      ItemRegistry::ItemID id = reader.Read<ItemRegistry::ItemID>();
      itemMap[key] = id;
    }
  }

  // Deserialize world
  u32 width = reader.Read<u32>();
  u32 height = reader.Read<u32>();
  mTerrain.Resize(width, height);

  u64 terrainSize = sizeof(Terrain::Cell) * width * height;
  const u8* source = reader.ReadBytes(terrainSize);
  memcpy(mTerrain.mTerrainCells, source, terrainSize);

  for (u32 y = 0; y < height; y++) {
    for (u32 x = 0; x < width; x++) {
      for (auto& listener : mTerrain.mChangeListeners) {
        WorldPos pos{ x, y };
        listener->OnTileChanged(pos);
        listener->OnWallChanged(pos);
      }
    }
  }
  return true;
}

// -------------------------------------------------------------------------- //

bool
World::ToBytes(alflib::MemoryWriter& writer) const
{
  // Write tile table
  writer.Write(u32(TileRegistry::Instance().GetRegistryMap().size()));
  for (const auto& entry : TileRegistry::Instance().GetRegistryMap()) {
    writer.Write(entry.first);
    writer.Write(entry.second);
  }

  // Write wall table
  writer.Write(u32(WallRegistry::Instance().GetRegistryMap().size()));
  for (const auto& entry : WallRegistry::Instance().GetRegistryMap()) {
    writer.Write(entry.first);
    writer.Write(entry.second);
  }

  // Write item table
  writer.Write(u32(ItemRegistry::Instance().GetRegistryMap().size()));
  for (const auto& entry : ItemRegistry::Instance().GetRegistryMap()) {
    writer.Write(entry.first);
    writer.Write(entry.second);
  }

  // Write dimensions
  writer.Write(mTerrain.mWidth);
  writer.Write(mTerrain.mHeight);

  // Write data
  writer.WriteBytes((u8*)mTerrain.mTerrainCells,
                    sizeof(Terrain::Cell) * mTerrain.mWidth * mTerrain.mHeight);

  return true;
}

// -------------------------------------------------------------------------- //

World
World::FromBytes(alflib::MemoryReader& reader)
{
  World world;
  world.Load(reader);
  return world;
}

f64
World::GetTime() const
{
  static dutil::Stopwatch sw{};
  return sw.fnow_s();
}
}
