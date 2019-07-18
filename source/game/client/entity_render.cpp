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

  if (renderer.GetDebugDrawCollision()) {
    renderer.GetDebugDraw().Begin(&camera);
    graphics::DebugDraw::Rect rect{};
    for (const auto entity : view) {
      Moveable& moveable = view.get<Moveable>(entity);
      Collideable& col = moveable.collideable;

      if (col.type == CollisionType::kRect) {
        auto* colrect = reinterpret_cast<const CollideableRect*>(&col);
        rect.position.x = MeterToPixel(moveable.position.x);
        rect.position.y = MeterToPixel(moveable.position.y);
        rect.size.x = MeterToPixel(colrect->rect.width);
        rect.size.y = MeterToPixel(colrect->rect.height);
        rect.color = alflib::Color{ 255, 20, 240 };
        rect.color.Alpha() = 255;
      }
      renderer.GetDebugDraw().Submit(rect);
    }
    renderer.GetDebugDraw().End();
  }
}

}
