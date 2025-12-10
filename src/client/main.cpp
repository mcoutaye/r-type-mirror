/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** main
*/

#include "engine/ecs/ecs.hpp"
#include "engine/systems/Components.hpp"
#include "engine/systems/RessourceManager.hpp"
#include "engine/systems/RenderSystem.hpp"
#include "engine/systems/InputSystem.hpp"
#include "engine/systems/CollisionSystem.hpp"
#include "engine/systems/WaveSystem.hpp"
#include "engine/systems/MovementSystem.hpp"
#include "engine/systems/StageFactory.hpp"
#include "engine/systems/EntityFactory.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));
    
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "R-TYPE - CLIENT", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    ECS ecs;

    // === CHARGEMENT DES TEXTURES ===
    auto& rm = ResourceManager::getInstance();
    rm.loadTexture("player", "assets/player.png");
    rm.loadTexture("player", "assets/ship.png");
    rm.loadTexture("enemy", "assets/enemy.png");
    rm.loadTexture("bullet", "assets/bullet.png");

    // === CRÉATION DU STAGE ===
    Factory::createStarfield(ecs, 150, 1920.f, 1080.f);
    Factory::createScreenBorders(ecs, 1920.f, 1080.f, 20.f);
    
    // Obstacles custom
    Factory::createObstacle(ecs, 800.f, 200.f, 100.f, 150.f);
    Factory::createObstacle(ecs, 1200.f, 600.f, 80.f, 200.f);
    Factory::createObstacle(ecs, 1500.f, 300.f, 120.f, 100.f);
    
    // Grilles de tuiles destructibles
    Factory::createTileGrid(ecs, 600.f, 400.f, 3, 2, 50.f, 50.f, 50);
    Factory::createTileGrid(ecs, 1000.f, 500.f, 3, 2, 50.f, 50.f, 50);

    // === SYSTÈMES ===
    RenderSystem    render(ecs, window);
    InputSystem     input(ecs);
    CollisionSystem collision(ecs);
    WaveSystem      waves(ecs);
    MovementSystem  movementSystem(ecs);

    // === CRÉATION DU JOUEUR ===
    Entity player = Factory::createPlayer(ecs, 100.f, 540.f, 0, "player");
    std::cout << "Joueur créé !\n";

    // === CHARGEMENT DU LEVEL ===
    std::vector<WaveData> level = {
        {3.0f,  "enemy", 6,  2000.f, 200.f},
        {8.0f,  "enemy", 10, 2000.f, 400.f},
        {15.0f, "enemy", 15, 2000.f, 600.f}
    };
    waves.loadLevel(level);

    // === BOUCLE PRINCIPALE ===
    sf::Clock clock;
    float shootCooldown = 0.f;
    const float SHOOT_DELAY = 0.2f;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
        }

        float dt = clock.restart().asSeconds();
        ecs.addTime(dt);
        shootCooldown -= dt;

        // Update systems
        input.update(dt);
        movementSystem.update(dt);
        waves.update(dt);
        collision.update(dt);

        // === GESTION DU TIR ===
        for (Entity e : ecs.getEntitiesByComponents<PlayerController, Position>()) {
            auto* ctrl = ecs.getComponent<PlayerController>(e);
            auto* pos = ecs.getComponent<Position>(e);
            
            if (ctrl && ctrl->isShooting && shootCooldown <= 0.f) {
                Factory::createProjectile(ecs, pos->x + 64.f, pos->y + 20.f, 800.f, 0.f, 1, 25, "bullet");
                shootCooldown = SHOOT_DELAY;
            }
        }

        // Mouvement des entités
        for (Entity e : ecs.getEntitiesByComponents<Position, Velocity>()) {
            auto* pos = ecs.getComponent<Position>(e);
            auto* vel = ecs.getComponent<Velocity>(e);
            pos->x += vel->x * dt;
            pos->y += vel->y * dt;
        }
        
        // Wrap des étoiles (scrolling infini)
        for (Entity e : ecs.getEntitiesByComponents<Star, Position>()) {
            auto* pos = ecs.getComponent<Position>(e);
            if (pos->x < -10.f) {
                pos->x = 1930.f;
                pos->y = static_cast<float>(rand() % 1080);
            }
        }
        
        // Supprimer les projectiles hors écran
        for (Entity e : ecs.getEntitiesByComponents<Projectile, Position>()) {
            auto* pos = ecs.getComponent<Position>(e);
            if (pos->x > 2000.f || pos->x < -100.f || pos->y > 1200.f || pos->y < -100.f) {
                ecs.killEntity(e);
            }
        }

        render.update(dt);
    }
    return 0;
}