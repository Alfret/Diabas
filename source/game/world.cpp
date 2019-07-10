#include "world.hpp"

namespace dib::game {

World::World()
  : mTerrain(*this, Terrain::Size::kLarge)
{}

// -------------------------------------------------------------------------- //

void
World::Update()
{
  network_.Update();
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
