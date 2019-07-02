#ifndef GENERIC_SYSTEM_HPP_
#define GENERIC_SYSTEM_HPP_

#include <entt/entt.hpp>
#include <dlog.hpp>
#include <optional>
#include "core/types.hpp"
#include "core/uuid.hpp"

namespace dib::system {

/**
 * Create an entity with component.
 *
 * @tparam TComponent Be pod structure, implement operator==,
 * implement operator<<
 * Note: The operator== must uniquely identify it among the entities
 * @return If the component didn't already exist and could be created.
 */
template <typename TComponent>
bool
Create(entt::registry& registry, const TComponent& component)
{
  const auto view = registry.view<TComponent>();

  bool found = false;
  for (const auto e : view) {
    if (component == view.get(e)) {
      found = true;
      break;
    }
  }

  if (!found) {
    DLOG_VERBOSE("created new entity [{}]", component);
    auto entity = registry.create();
    registry.assign<TComponent>(entity, component);
  } else {
    DLOG_WARNING("entity already exists, create failed");
  }

  return !found;
}

/**
 * Replace component from existing entity. Uses operator== to identify it.
 *
 * @tparam TComponent Be pod structure, implement operator==,
 * implement operator<<
 * Note: The operator== must uniquely identify it among the entities
 * @return If the component was found and could be replaced.
 */
template <typename TComponent>
bool
Replace(entt::registry& registry, const TComponent& component)
{
  const auto view = registry.view<TComponent>();
  bool found = false;

  u32 entity;
  for (const u32 e : view) {
    if (component == view.get(e)) {
      found = true;
      entity = e;
      break;
    }
  }

  if (found) {
    DLOG_VERBOSE("replaced component for [{}]", component);
    registry.replace<TComponent>(entity, component);
  } else {
    DLOG_WARNING("could not find entity with component, replace failed");
  }

  return found;
}

/**
 * Delete if entity with TComponent.uuid matches uuid, else noop.
 *
 * @tparam TComponent Be pod structure, have a Uuid named uuid.
 */
template <typename TComponent>
void
Delete(entt::registry& registry, const Uuid& uuid)
{
  const auto view = registry.view<TComponent>();
  for (const u32 entity : view) {
    if (uuid == view.get(entity).uuid) {
      registry.destroy(entity);
      break;
    }
  }
}

/**
 * Delete all entities that has the given component.
 */
template<typename TComponent>
void DeleteEntitiesWithComponent(entt::registry& registry)
{
  for (const auto entity : registry.view<TComponent>()) {
    registry.destroy(entity);
  }
}

/**
 * @tparam TComponent must have a uuid member.
 */
template <typename TComponent>
std::optional<const TComponent*>
ComponentFromUuid(entt::registry& registry, const Uuid& uuid)
{
  const auto view = registry.view<TComponent>();
  for (const auto entity : view) {
    if (uuid == view.get(entity).uuid) {
      return &view.get(entity);
    }
  }
  return std::nullopt;
}

/**
 * Count the number of entities active that has the given component.
 */
template <typename TComponent>
u64
CountEntities(entt::registry& registry)
{
  const auto view = registry.view<TComponent>();
  u64 count = 0;
  for (const auto _ : view) {
    ++count;
  }
    return count;
}

}

#endif//GENERIC_SYSTEM_HPP_
