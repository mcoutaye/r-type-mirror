/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Collision System
*/

#include "engine/systems/CollisionSystem.hpp"
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
            sf::FloatRect rect1(pos1->x - coll1->width/2, pos1->y - coll1->height/2, coll1->width, coll1->height);
            sf::FloatRect rect2(pos2->x - coll2->width/2, pos2->y - coll2->height/2, coll2->width, coll2->height);

            if (rect1.intersects(rect2)) {
                // Vérifie si l'une des entités est un projectile
                auto* proj1 = _ecs.getComponent<Projectile>(e1);
                auto* proj2 = _ecs.getComponent<Projectile>(e2);

                if (proj1) {
                // e1 est un projectile : applique les dégâts à e2 si c'est un ennemi (équipe différente)
                auto* h2 = _ecs.getComponent<Health>(e2);
                if (h2 && coll1->team != coll2->team) {
                    h2->current -= proj1->damage;
                    toDestroy.push_back(e1);  // Détruit le projectile
                    if (h2 && h2->current <= 0) {
                        toDestroy.push_back(e2);  // Détruit l'ennemi

                        // 20% de chance de drop un power-up
                        if (rand() % 100 < 20) {
                            Entity powerUp = _ecs.createEntity();
                            _ecs.addComponent(powerUp, Position{pos2->x, pos2->y});
                            _ecs.addComponent(powerUp, Drawable{"powerup.png", {0, 0, 32, 32}, 15, true});

                            // Choisis un type de power-up aléatoirement
                            PowerUp::Type type = PowerUp::Type::TripleShot;  // Pour l'instant, on ne gère que le triple shot
                            _ecs.addComponent(powerUp, PowerUp{type, 10.f});  // Effet pendant 10 secondes

                            std::cout << "[PowerUp] Power-up spawned at (" << pos2->x << ", " << pos2->y << ")" << std::endl;
                        }
                    }
                }
            } else if (proj2) {
                // e2 est un projectile : applique les dégâts à e1 si c'est un ennemi
                auto* h1 = _ecs.getComponent<Health>(e1);
                if (h1 && coll1->team != coll2->team) {
                    h1->current -= proj2->damage;
                    toDestroy.push_back(e2);
                    if (h1->current <= 0) {
                        toDestroy.push_back(e1);  // Détruit l'ennemi
                    }
                }
                } else {
                    // Collision classique (joueur/ennemi)
                    auto* h1 = _ecs.getComponent<Health>(e1);
                    auto* h2 = _ecs.getComponent<Health>(e2);
                    if (h1 && h2 && coll1->team != coll2->team) {
                        h1->current -= coll2->damage;
                        h2->current -= coll1->damage;
                        if (h1->current <= 0) toDestroy.push_back(e1);
                        if (h2->current <= 0) toDestroy.push_back(e2);
                    }
                }
            }
        }
    }

    // Supprime les entités marquées
    for (Entity e : toDestroy) {
        _ecs.killEntity(e);
    }
}
