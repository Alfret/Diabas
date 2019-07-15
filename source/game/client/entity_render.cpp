#include "game/client/entity_render.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/physics/units.hpp"
#include "game/client/render_component.hpp"
#include "game/gameplay/moveable.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace dib::game {

void
RenderEntities(graphics::Renderer& renderer,
               const graphics::Camera& camera,
               EntityManager& entityManager)
{
  renderer.GetSpriteBatch().Begin(&camera);

  // Retrieve views of entities
  auto view = entityManager.GetRegistry().view<Moveable, RenderComponent>();

  // Render each entity
  for (auto entity : view) {
    Moveable& movableComponent = view.get<Moveable>(entity);
    RenderComponent& renderComponent = view.get<RenderComponent>(entity);

    renderer.GetSpriteBatch().Submit(
      renderComponent.texture,
      Vector3F{ MeterToPixel(movableComponent.position.x),
                MeterToPixel(movableComponent.position.y),
                0.5f },
      Vector2F{ MeterToPixel(movableComponent.width),
                MeterToPixel(movableComponent.height) });
  }

  // Done
  renderer.GetSpriteBatch().End();
}

}
