#include "entity_manager.hpp"

namespace dib {

EntityManager::EntityManager() {}

void
EntityManager::Clear()
{
  registry_.reset();
}
}
