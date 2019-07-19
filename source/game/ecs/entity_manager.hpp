#ifndef ENTITY_MANAGER_HPP_
#define ENTITY_MANAGER_HPP_

#include <entt/entt.hpp>

namespace dib {

using Entity = entt::entity;

class EntityManager
{
public:
  EntityManager();

  entt::registry& GetRegistry() { return registry_; }
  const entt::registry& GetRegistry() const { return registry_; }

private:
  entt::registry registry_{};
};
}

#endif // ENTITY_MANAGER_HPP_
