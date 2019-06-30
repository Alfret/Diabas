#include "player_system.hpp"

#include "network/side.hpp"
#include <dlog.hpp>
#include "network/connection_state.hpp"

namespace dib::system {

bool
PlayerDataCreate(entt::registry& registry, const PlayerData& player_data)
{
  auto view = registry.view<PlayerData>();

  bool found = false;
  for (u32 e : view) {
    if (player_data == view.get(e)) {
      found = true;
      break;
    }
  }

  if (!found) {
    DLOG_VERBOSE("created new PlayerData for [{}]", player_data);
    auto entity = registry.create();
    registry.assign<PlayerData>(entity, player_data);
  } else {
    DLOG_WARNING("could not create PlayerData, entry already exists");
  }

  return !found;
}

bool
PlayerDataUpdate(entt::registry& registry,
                 const PlayerData& player_data)
{
  auto view = registry.view<PlayerData>();
  bool found = false;

  u32 entity;
  for (u32 e : view) {
    if (player_data == view.get(e)) {
      found = true;
      entity = e;
      break;
    }
  }

  if (found) {
    DLOG_VERBOSE("updated PlayerData for [{}]", player_data);
    registry.replace<PlayerData>(entity, player_data);
  } else {
    DLOG_WARNING("could not find PlayerData entry, update failed.");
  }

  return found;
}

void
PlayerDataDelete(entt::registry& registry, const Uuid& uuid)
{
  auto view = registry.view<PlayerData>();
  for (u32 entity : view) {
    if (uuid == view.get(entity).uuid) {
      registry.destroy(entity);
      break;
    }
  }
}

void
PlayerDataDelete(entt::registry& registry, const Uuid* uuid)
{
  AlfAssert(uuid, "uuid is null");
  auto view = registry.view<PlayerData>();
  for (u32 entity : view) {
    if (*uuid == view.get(entity).uuid) {
      registry.destroy(entity);
      break;
    }
  }
}

void
PlayerDataDelete(entt::registry& registry, const PlayerData& player_data)
{
  PlayerDataDelete(registry, player_data.uuid);
}

void
PlayerDataDelete(entt::registry& registry, const PlayerData* player_data)
{
  AlfAssert(player_data, "player_data is null");
  PlayerDataDelete(registry, player_data->uuid);
}

std::optional<const PlayerData*>
PlayerDataFromConnectionId(entt::registry& registry, const ConnectionId connection_id) {
  auto view = registry.view<PlayerData>();
  for (u32 entity : view) {
    if (connection_id == view.get(entity).connection_id) {
      return &view.get(entity);
    }
  }
  return std::nullopt;
}

std::optional<const PlayerData*>
PlayerDataFromUuid(entt::registry& registry, const Uuid& uuid)
{
  auto view = registry.view<PlayerData>();
  for (u32 entity : view) {
    if (uuid == view.get(entity).uuid) {
      return &view.get(entity);
    }
  }
  return std::nullopt;
}

}
