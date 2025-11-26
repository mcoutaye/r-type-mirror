
// InputSystem.cpp
#include "InputSystem.hpp"
#include <SFML/Graphics.hpp>
#include "Components.hpp"

InputSystem::InputSystem(ECS& ecs) : ISystem(ecs) {}

void InputSystem::update(double dt) {
    auto entities = _ecs.getEntitiesByComponents<PlayerController, Position, Velocity>();

    for (Entity e : entities) {
        auto* ctrl = _ecs.getComponent<PlayerController>(e);
        auto* pos = _ecs.getComponent<Position>(e);
        auto* vel = _ecs.getComponent<Velocity>(e);

        // Mouvements
        vel->x = 0.f;
        vel->y = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) vel->y -= 300.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) vel->y += 300.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) vel->x -= 300.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) vel->x += 300.f;

        // Shoot (avec cooldown)
        ctrl->isShooting = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
        if (ctrl->isShooting && _shootCooldown[ctrl->playerId] <= 0.f) {
            // TODO: SPAWN BULLET ici
            _shootCooldown[ctrl->playerId] = 0.1f;
        }
        _shootCooldown[ctrl->playerId] -= dt;
    }
}