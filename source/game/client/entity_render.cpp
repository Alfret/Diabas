#include "game/client/entity_render.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "game/physics/units.hpp"
#include "game/client/render_component.hpp"
#include "game/gameplay/moveable.hpp"
#include "game/physics/path_finding.hpp"

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
      Vector2F{ movableComponent.width, movableComponent.height });
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

  auto astar_view = entityManager.GetRegistry().view<AStarPathDebug>();
  if (astar_view.size() > 0) {
    renderer.GetDebugDraw().Begin(&camera);

    for (const auto entity : astar_view) {
      auto& astar = astar_view.get(entity);
      for (u32 i = 0; i < astar.path.size(); i++) {
        graphics::DebugDraw::Circle circle{};
        circle.position =
          Position{ TileToPixel(astar.path[i].X()) + TileToPixel(1) / 2,
                    TileToPixel(astar.path[i].Y()) + TileToPixel(1) / 2 };
        circle.radius = 3.5f;
        circle.color = alflib::Color{ 20, 255, 10 };
        circle.color.Alpha() = 255;
        renderer.GetDebugDraw().Submit(circle);
      }
    }

    renderer.GetDebugDraw().End();
  }
}

}
