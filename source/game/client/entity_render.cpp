#include "game/client/entity_render.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/client/render_component.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::game {

void
RenderEntities(EntityManager& entityManager)
{
  auto view = entityManager.GetRegistry().view<RenderComponent>();
  for (auto entity : view) {
    RenderComponent& renderComponent = view.get(entity);
  }
}

}