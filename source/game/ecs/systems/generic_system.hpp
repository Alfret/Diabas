#ifndef GENERIC_SYSTEM_HPP_
#define GENERIC_SYSTEM_HPP_

#include <entt/entt.hpp>
#include <dlog.hpp>
#include <optional>
#include "core/types.hpp"
#include "core/uuid.hpp"
#include "game/ecs/components/player_data_component.hpp"
#include "network/side.hpp"

namespace dib::system {

/**
 * Create an entity with component, fails if component already exists.
 *
 * @tparam TComponent Be pod structure, implement operator==
 * Note: The operator== must uniquely identify it among the entities
 * @return
 */
template<typename TComponent>
std::optional<entt::entity>
SafeCreate(entt::registry& registry, const TComponent& component)
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
    auto entity = registry.create();
    registry.assign<TComponent>(entity, component);
    return entity;
  } else {
    DLOG_WARNING("entity already exists, create failed");
    return std::nullopt;
  }
}

// -------------------------------------------------------------------------- //

template<typename TComponent>
void
Assign(entt::registry& registry,
       entt::registry::entity_type entity,
       const TComponent& component)
{
  registry.assign<TComponent>(entity, component);
}

// -------------------------------------------------------------------------- //

/**
 * Replace component from existing entity. Uses operator== to identify it.
 *
 * @tparam TComponent Be pod structure, implement operator==
 * Note: The operator== must uniquely identify it among the entities
 * @return If the component was found and could be replaced.
 */
template<typename TComponent>
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
    registry.replace<TComponent>(entity, component);
  } else {
    DLOG_WARNING("could not find entity with component, replace failed");
  }

  return found;
}

template<>
inline bool
Replace(entt::registry& registry, const PlayerData& player_data)
{
  const auto view = registry.view<PlayerData>();
  bool found = false;

  u32 entity;
  for (const u32 e : view) {
    const auto current_pd = view.get(e);
    if (player_data == current_pd) {
      // Server must also case about the connection_id field.
      if constexpr (kSide == Side::kServer) {
        AlfAssert(
          player_data.connection_id == current_pd.connection_id,
          "Attempting to update PlayerData, but connection_id is not matching. "
          "It is likely it was not set (and you have to set it manually).");
      }
      found = true;
      entity = e;
      break;
    }
  }

  if (found) {
    registry.replace<PlayerData>(entity, player_data);
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
template<typename TComponent>
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
void
DeleteEntitiesWithComponent(entt::registry& registry)
{
  for (const auto entity : registry.view<TComponent>()) {
    registry.destroy(entity);
  }
}

/**
 * @tparam TComponent must have a uuid member.
 */
template<typename TComponent>
std::optional<u32>
EntityFromUuid(entt::registry& registry, const Uuid& uuid)
{
  const auto view = registry.view<TComponent>();
  for (const auto entity : view) {
    if (uuid == view.get(entity).uuid) {
      return entity;
    }
  }
  return std::nullopt;
}

/**
 * @tparam TComponent must have a uuid member.
 */
template<typename TComponent>
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
template<typename TComponent>
u64
CountEntities(entt::registry& registry)
{
  const auto view = registry.view<TComponent>();
  u64 count = 0;
  for ([[maybe_unused]] const auto _ : view) {
    ++count;
  }
  return count;
}

}

#endif // GENERIC_SYSTEM_HPP_
