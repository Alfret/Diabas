#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/ecs/entity_manager.hpp"
#include "graphics/renderer.hpp"
#include "game/npc/npc_registry.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::game {

/** Render all entities **/
void
RenderEntities(graphics::Renderer& renderer,
               const graphics::Camera& camera,
               EntityManager& entityManager);

void
RenderNpcs(graphics::Renderer& renderer,
           const graphics::Camera& camera,
           NpcRegistry& npc_registry);
}
