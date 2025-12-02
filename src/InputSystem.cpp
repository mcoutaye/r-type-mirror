
// InputSystem.cpp
#include "InputSystem.hpp"
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && _shootCooldown[ctrl->playerId] <= 0.f) {
            // Crée un projectile
            Entity projectile = _ecs.createEntity();
            _ecs.addComponent(projectile, Position{pos->x + 30.f, pos->y});  // Position devant le joueur
            _ecs.addComponent(projectile, Velocity{400.f, 0.f});  // Vitesse vers la droite
            _ecs.addComponent(projectile, Drawable{"bullet.png", {0, 0, 16, 8}, 5, true});
            _ecs.addComponent(projectile, Collider{10.f, 5.f, true, 1, 10});  // Équipe 1 (joueur)
            _ecs.addComponent(projectile, Projectile{400.f, 40});  // 20 dégâts, tiré par le joueur

            _shootCooldown[ctrl->playerId] = 0.2f;  // Cooldown de 0.2 seconde
        }
        _shootCooldown[ctrl->playerId] -= dt;
    }
}