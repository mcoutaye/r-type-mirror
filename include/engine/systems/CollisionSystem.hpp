/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Collision System
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"

class CollisionSystem : public ISystem {
public:
    CollisionSystem(ECS& ecs);
    void update(double dt) override;
};

CollisionSystem::CollisionSystem(ECS& ecs) : ISystem(ecs) {}

void CollisionSystem::update(double dt) {
    auto entities = _ecs.getEntitiesByComponents<Position_t, Collider_t>();
    std::vector<Entity> toDestroy;

    for (size_t i = 0; i < entities.size(); ++i) {
        Entity e1 = entities[i];
        auto* pos1 = _ecs.getComponent<Position_t>(e1);
        auto* coll1 = _ecs.getComponent<Collider_t>(e1);

        for (size_t j = i + 1; j < entities.size(); ++j) {
            Entity e2 = entities[j];
            auto* pos2 = _ecs.getComponent<Position_t>(e2);
            auto* coll2 = _ecs.getComponent<Collider_t>(e2);

            // AABB collision
            sf::FloatRect rect1(pos1->x - coll1->width/2, pos1->y - coll1->height/2, coll1->width, coll1->height);
            sf::FloatRect rect2(pos2->x - coll2->width/2, pos2->y - coll2->height/2, coll2->width, coll2->height);

            if (rect1.intersects(rect2)) {
                // Vérifie si l'une des entités est un projectile
                auto* proj1 = _ecs.getComponent<Projectile_t>(e1);
                auto* proj2 = _ecs.getComponent<Projectile_t>(e2);

                if (proj1) {
                // e1 est un projectile : applique les dégâts à e2 si c'est un ennemi (équipe différente)
                auto* h2 = _ecs.getComponent<Health_t>(e2);
                if (h2 && coll1->team != coll2->team) {
                    h2->current -= proj1->damage;
                    toDestroy.push_back(e1);  // Détruit le projectile
                    if (h2->current <= 0) {
                        toDestroy.push_back(e2);  // Détruit l'ennemi
                    }
                }
            } else if (proj2) {
                // e2 est un projectile : applique les dégâts à e1 si c'est un ennemi
                auto* h1 = _ecs.getComponent<Health_t>(e1);
                if (h1 && coll1->team != coll2->team) {
                    h1->current -= proj2->damage;
                    toDestroy.push_back(e2);  // Détruit le projectile
                    if (h1->current <= 0) {
                        toDestroy.push_back(e1);  // Détruit l'ennemi
                    }
                }
                } else {
                    // Collision classique (joueur/ennemi)
                    auto* h1 = _ecs.getComponent<Health_t>(e1);
                    auto* h2 = _ecs.getComponent<Health_t>(e2);
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
