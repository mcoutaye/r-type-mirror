/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Movement System
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include <cmath>

class MovementSystem : public ISystem {
public:
    MovementSystem(ECS& ecs) : ISystem(ecs) {}
    void update(double dt) override;
private:
    float _totalTime = 0.f;
};

void MovementSystem::update(double dt) {
    auto entities = _ecs.getEntitiesByComponents<Position_t, Velocity_t, MovementPattern_t>();
    _totalTime += static_cast<float>(dt);
    float time = _totalTime;

    for (Entity e : entities) {
        auto* pos = _ecs.getComponent<Position_t>(e);
        auto* vel = _ecs.getComponent<Velocity_t>(e);
        auto* pattern = _ecs.getComponent<MovementPattern_t>(e);

        // Sauvegarde la vélocité de base
        float baseX = vel->x;
        float baseY = vel->y;

        // Applique le pattern de mouvement
        switch (pattern->type) {
            case MovementPattern_t::Type::Linear:
                // Mouvement linéaire (géré par MoveSystem)
                break;

            case MovementPattern_t::Type::Sinus:
                // Mouvement en onde sinusoïdale (haut/bas)
                pos->y += sin(time * pattern->frequency) * pattern->amplitude * dt;
                break;

            case MovementPattern_t::Type::Cosinus:
                // Mouvement en onde cosinusoïdale (haut/bas, déphasé par rapport à sinus)
                pos->y += cos(time * pattern->frequency) * pattern->amplitude * dt;
                break;

            case MovementPattern_t::Type::Circle:
                // Mouvement circulaire (autour d'un point central)
                pos->x += cos(time * pattern->speed) * pattern->radius * dt;
                pos->y += sin(time * pattern->speed) * pattern->radius * dt;
                break;

            case MovementPattern_t::Type::Zigzag:
                // Mouvement en zigzag (gauche/droite + avance)
                // pos->x += baseX * dt; // Géré par MoveSystem
                pos->y += sin(time * pattern->speed) * pattern->amplitude * dt;
                break;

            case MovementPattern_t::Type::Spiral:
                // Mouvement en spirale (éloignement progressif du centre)
                pos->x += cos(time * pattern->speed) * (pattern->radius + time * 0.1f) * dt;
                pos->y += sin(time * pattern->speed) * (pattern->radius + time * 0.1f) * dt;
                break;
        }

        // Applique la vélocité de base (sauf pour les patterns qui gèrent déjà x/y)
        // if (pattern->type == MovementPattern_t::Type::Linear ||
        //     pattern->type == MovementPattern_t::Type::Zigzag) {
        //     pos->x += baseX * dt;
        // }
        // if (pattern->type == MovementPattern_t::Type::Linear) {
        //     pos->y += baseY * dt;
        // }
    }
}