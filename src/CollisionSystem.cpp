// CollisionSystem.cpp
#include "CollisionSystem.hpp"
#include "Components.hpp"

CollisionSystem::CollisionSystem(ECS& ecs) : ISystem(ecs) {}

void CollisionSystem::update(double dt) {
    auto entities = _ecs.getEntitiesByComponents<Position, Collider>();

    for (size_t i = 0; i < entities.size(); ++i) {
        Entity e1 = entities[i];
        auto* pos1 = _ecs.getComponent<Position>(e1);
        auto* coll1 = _ecs.getComponent<Collider>(e1);

        for (size_t j = i + 1; j < entities.size(); ++j) {
            Entity e2 = entities[j];
            auto* pos2 = _ecs.getComponent<Position>(e2);
            auto* coll2 = _ecs.getComponent<Collider>(e2);

            // AABB collision
            sf::FloatRect rect1(pos1->x - coll1->width/2, pos1->y - coll1->height/2,
                                coll1->width, coll1->height);
            sf::FloatRect rect2(pos2->x - coll2->width/2, pos2->y - coll2->height/2,
                                coll2->width, coll2->height);

            if (rect1.intersects(rect2)) {
                // Collision détectée !
                if (coll1->team != coll2->team) {  // equipes différentes
                    auto* h1 = _ecs.getComponent<Health>(e1);
                    auto* h2 = _ecs.getComponent<Health>(e2);
                    if (h1) h1->current -= coll2->damage;
                    if (h2) h2->current -= coll1->damage;

                    // Mort ?
                    if (h1 && h1->current <= 0) _ecs.killEntity(e1);
                    if (h2 && h2->current <= 0) _ecs.killEntity(e2);
                }
            }
        }
    }
}