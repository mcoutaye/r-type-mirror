/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Movement System
*/

#include "ecs.hpp"

void MovementSystem::update(double dt) {
    auto entities = _ecs.getEntitiesByComponents<Position, Velocity, MovementPattern>();
    float time = static_cast<float>(_ecs.getTime());  // Temps écoulé

    for (Entity e : entities) {
        auto* pos = _ecs.getComponent<Position>(e);
        auto* vel = _ecs.getComponent<Velocity>(e);
        auto* pattern = _ecs.getComponent<MovementPattern>(e);

        // Sauvegarde la vélocité de base
        float baseX = vel->x;
        float baseY = vel->y;

        // Applique le pattern de mouvement
        switch (pattern->type) {
            case MovementPattern::Type::Linear:
                // Mouvement linéaire (pas de modification, utilise juste la vélocité de base)
                break;

            case MovementPattern::Type::Sinus:
                // Mouvement en onde sinusoïdale (haut/bas)
                pos->y += sin(time * pattern->frequency) * pattern->amplitude * dt;
                break;

            case MovementPattern::Type::Cosinus:
                // Mouvement en onde cosinusoïdale (haut/bas, déphasé par rapport à sinus)
                pos->y += cos(time * pattern->frequency) * pattern->amplitude * dt;
                break;

            case MovementPattern::Type::Circle:
                // Mouvement circulaire (autour d'un point central)
                pos->x += cos(time * pattern->speed) * pattern->radius * dt;
                pos->y += sin(time * pattern->speed) * pattern->radius * dt;
                break;

            case MovementPattern::Type::Zigzag:
                // Mouvement en zigzag (gauche/droite + avance)
                pos->x += baseX * dt;
                pos->y += sin(time * pattern->speed) * pattern->amplitude * dt;
                break;

            case MovementPattern::Type::Spiral:
                // Mouvement en spirale (éloignement progressif du centre)
                pos->x += cos(time * pattern->speed) * (pattern->radius + time * 0.1f) * dt;
                pos->y += sin(time * pattern->speed) * (pattern->radius + time * 0.1f) * dt;
                break;
        }

        // Applique la vélocité de base (sauf pour les patterns qui gèrent déjà x/y)
        if (pattern->type == MovementPattern::Type::Linear ||
            pattern->type == MovementPattern::Type::Zigzag) {
            pos->x += baseX * dt;
        }
        if (pattern->type == MovementPattern::Type::Linear) {
            pos->y += baseY * dt;
        }
    }
}