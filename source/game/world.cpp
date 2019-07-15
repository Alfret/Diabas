#include "world.hpp"
#include "game/gameplay/moveable.hpp"
#include <dutil/stopwatch.hpp>
#include <microprofile/microprofile.h>

namespace dib::game {

World::World(const TileRegistry& tileRegistry)
  : mTileRegistry(tileRegistry)
  , mTerrain(mTileRegistry, *this, Terrain::Size::kLarge)
{}

// -------------------------------------------------------------------------- //

void
World::Update(const f64 delta)
{
  MICROPROFILE_SCOPEI("world", "update", MP_BLUE);
  network_.Update();
  UpdateMoveables(*this, delta);
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
