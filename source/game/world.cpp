#include "world.hpp"
#include "game/physics/moveable.hpp"
#include <dutil/stopwatch.hpp>

namespace dib::game {

World::World(const TileRegistry& tileRegistry)
  : mTileRegistry(tileRegistry)
  , mTerrain(mTileRegistry, *this, Terrain::Size::kLarge)
{}

// -------------------------------------------------------------------------- //

void
World::Update(const f64 delta)
{
  network_.Update();
  dutil::FixedTimeUpdate(60, [&](){SimulateMoveables(*this, 1/60.0);});
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

dib::EntityManager&
World::GetEntityManager()
{
  return entity_manager_;
}
}
