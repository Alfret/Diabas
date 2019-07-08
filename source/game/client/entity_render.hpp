#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/ecs/entity_manager.hpp"
#include "graphics/renderer.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::game {

/** Render all entities **/
void
RenderEntities(graphics::Renderer& renderer,
               const graphics::Camera& camera,
               EntityManager& entityManager);

}