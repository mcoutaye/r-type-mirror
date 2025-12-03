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
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "R-TYPE - CLIENT", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    ECS ecs;

    // === CHARGEMENT DES TEXTURES (OBLIGATOIRE) ===
    auto& rm = ResourceManager::getInstance();
    if (!rm.loadTexture("player", "assets/player.png") &&
        !rm.loadTexture("player", "assets/ship.png")) {
        std::cerr << "ERREUR : impossible de charger player.png ou ship.png\n";
    }
    if (!rm.loadTexture("enemy", "assets/enemy.png")) {
        std::cerr << "ERREUR : impossible de charger enemy.png\n";
    }
    if (!rm.loadTexture("bullet", "assets/bullet.png")) {
        std::cerr << "INFO : bullet.png non trouvé, utilisation texture par défaut\n";
    }

    std::cout << "Texture player chargée ? "
          << rm.loadTexture("player", "assets/player.png") << "\n";
    std::cout << "Texture player chargée ? "
            << rm.loadTexture("player", "assets/ship.png") << "\n";
    std::cout << "Texture enemy chargée ? "
            << rm.loadTexture("enemy", "assets/enemy.png") << "\n";

    // === SYSTÈMES ===
    RenderSystem    render(ecs, window);
    InputSystem     input(ecs);
    CollisionSystem collision(ecs);
    WaveSystem      waves(ecs);
    MovementSystem movementSystem(ecs);

    // === CRÉATION DU JOUEUR ===
    Entity player = ecs.createEntity();
    ecs.addComponent(player, Position{100.f, 100.f});
    ecs.addComponent(player, PlayerController{});
    ecs.addComponent(player, Velocity{0.f, 0.f});
    ecs.addComponent(player, Drawable{"player.png", {0, 0, 64, 64}, 10, true});
    ecs.addComponent(player, Collider{100.f, 70.f, true, 1, 50});
    ecs.addComponent(player, Health{200, 200});

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
    const float SHOOT_DELAY = 0.2f;  // 200ms entre chaque tir
    
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

        input.update(dt);
        movementSystem.update(dt);
        waves.update(dt);
        collision.update(dt);

        // === GESTION DU TIR ===
        for (Entity e : ecs.getEntitiesByComponents<PlayerController, Position>()) {
            auto* ctrl = ecs.getComponent<PlayerController>(e);
            auto* pos = ecs.getComponent<Position>(e);
            
            if (ctrl && ctrl->isShooting && shootCooldown <= 0.f) {
                // Créer un projectile
                Entity bullet = ecs.createEntity();
                ecs.addComponent(bullet, Position{pos->x + 64.f, pos->y + 20.f});
                ecs.addComponent(bullet, Velocity{800.f, 0.f});  // Vitesse vers la droite
                ecs.addComponent(bullet, Drawable{"bullet", {0, 0, 16, 8}, 20, true, 1.f, 0.f});
                ecs.addComponent(bullet, Collider{16.f, 8.f, false, 1, 25});  // team 1 = joueur
                ecs.addComponent(bullet, Projectile{800.f, 25});
                
                shootCooldown = SHOOT_DELAY;
            }
        }

        // Mouvement simple
        for (Entity e : ecs.getEntitiesByComponents<Position, Velocity>()) {
            auto* pos = ecs.getComponent<Position>(e);
            auto* vel = ecs.getComponent<Velocity>(e);
            pos->x += vel->x * dt;
            pos->y += vel->y * dt;
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