#include "MovementSystem.hpp"
#include <cmath>  // Pour sin() et cos()

void MovementSystem::update(double dt) {
    auto entities = _ecs.getEntitiesByComponents<Position, Velocity, MovementPattern>();
    for (Entity e : entities) {
        auto* pos = _ecs.getComponent<Position>(e);
        auto* vel = _ecs.getComponent<Velocity>(e);
        auto* pattern = _ecs.getComponent<MovementPattern>(e);

        // Mise à jour de la position en fonction du pattern
        float time = static_cast<float>(_ecs.getTime());  // Temps écoulé depuis le début
        switch (pattern->type) {
            case MovementPattern::Type::Sinus:
                pos->y += sin(time * pattern->frequency) * pattern->amplitude * dt;
                break;
            case MovementPattern::Type::Cosinus:
                pos->y += cos(time * pattern->frequency) * pattern->amplitude * dt;
                break;
            case MovementPattern::Type::Linear:
            default:
                // Mouvement linéaire (par défaut)
                break;
        }

        // Application de la vélocité de base
        pos->x += vel->x * dt;
        pos->y += vel->y * dt;
    }
}
