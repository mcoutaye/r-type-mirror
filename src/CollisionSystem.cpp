// CollisionSystem.cpp
#include "CollisionSystem.hpp"
#include "Components.hpp"
#include <iostream>

CollisionSystem::CollisionSystem(ECS& ecs) : ISystem(ecs) {}

void CollisionSystem::update(double dt) {
    auto entities = _ecs.getEntitiesByComponents<Position, Collider>();
    std::vector<Entity> toDestroy;

    for (size_t i = 0; i < entities.size(); ++i) {
        Entity e1 = entities[i];
        auto* pos1 = _ecs.getComponent<Position>(e1);
        auto* coll1 = _ecs.getComponent<Collider>(e1);

        for (size_t j = i + 1; j < entities.size(); ++j) {
            Entity e2 = entities[j];
            auto* pos2 = _ecs.getComponent<Position>(e2);
            auto* coll2 = _ecs.getComponent<Collider>(e2);

            // AABB collision
            sf::FloatRect rect1(
                pos1->x - coll1->width / 2,
                pos1->y - coll1->height / 2,
                coll1->width,
                coll1->height
            );
            sf::FloatRect rect2(
                pos2->x - coll2->width / 2,
                pos2->y - coll2->height / 2,
                coll2->width,
                coll2->height
            );

            // Vérifie la collision
            if (rect1.intersects(rect2)) {
                // Collision détectée entre équipes différentes
                if (coll1->team != coll2->team) {
                    auto* h1 = _ecs.getComponent<Health>(e1);
                    auto* h2 = _ecs.getComponent<Health>(e2);

                    // Applique les dégâts si les composants Health existent
                    if (h1) {
                        h1->current = std::max(0, h1->current - coll2->damage);
                        if (_ecs.hasComponent<PlayerController>(e1)) {
                            std::cout << "[Collision] PV du joueur : " << h1->current << "/" << h1->max << std::endl;
                        }
                    }
                    if (h2) {
                        h2->current = std::max(0, h2->current - coll1->damage);
                        if (_ecs.hasComponent<PlayerController>(e2)) {
                            std::cout << "[Collision] PV du joueur : " << h2->current << "/" << h2->max << std::endl;
                        }
                    }

                    // Marquer pour destruction si PV ≤ 0
                    if (h1 && h1->current <= 0) {
                        toDestroy.push_back(e1);
                    }
                    if (h2 && h2->current <= 0) {
                        toDestroy.push_back(e2);
                    }
                }
            }
        }
    }

    // Supprime les entités marquées après la boucle
    for (Entity e : toDestroy) {
        _ecs.killEntity(e);
    }
}
