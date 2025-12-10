#include "../../../include/engine/systems/PowerUpSystem.hpp"

void PowerUpSystem::update(double dt) {
    auto powerUps = _ecs.getEntitiesByComponents<Position, PowerUp, Drawable>();
    auto players = _ecs.getEntitiesByComponents<PlayerController, Position, Shootable>();

    for (Entity powerUp : powerUps) {
        auto* powerUpPos = _ecs.getComponent<Position>(powerUp);
        auto* powerUpComp = _ecs.getComponent<PowerUp>(powerUp);

        for (Entity player : players) {
            auto* playerPos = _ecs.getComponent<Position>(player);
            auto* shootable = _ecs.getComponent<Shootable>(player);

            // Définis les rectangles de collision
            sf::FloatRect playerRect(playerPos->x - 32.f, playerPos->y - 32.f, 64.f, 64.f);
            sf::FloatRect powerUpRect(powerUpPos->x - 16.f, powerUpPos->y - 16.f, 32.f, 32.f);

            // Vérifie la collision
            if (playerRect.intersects(powerUpRect)) {
                std::cout << "[PowerUp] Triple shot activé !" << std::endl;
                shootable->tripleShot = true;

                // Détruit le power-up après utilisation
                _ecs.killEntity(powerUp);
            }
        }
    }
}
