// src/main.cpp
#include <SFML/Graphics.hpp>
#include <iostream>
#include "ecs.hpp"
#include "Components.hpp"
#include "ResourceManager.hpp"
#include "RenderSystem.hpp"
#include "InputSystem.hpp"
#include "CollisionSystem.hpp"
#include "WaveSystem.hpp"

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

    // === CRÉATION DU JOUEUR ===
    Entity player = ecs.createEntity();
    ecs.addComponent(player, Position{300.f, 540.f});
    ecs.addComponent(player, Velocity{0.f, 0.f});
    ecs.addComponent(player, Drawable{"player", sf::IntRect(0,0,120,80), 20, true});
    ecs.addComponent(player, PlayerController{0});
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
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
        }

        float dt = clock.restart().asSeconds();

        input.update(dt);
        waves.update(dt);
        collision.update(dt);

        // Mouvement simple (tu peux faire un MovementSystem plus tard)
        for (Entity e : ecs.getEntitiesByComponents<Position, Velocity>()) {
            auto* pos = ecs.getComponent<Position>(e);
            auto* vel = ecs.getComponent<Velocity>(e);
            pos->x += vel->x * dt;
            pos->y += vel->y * dt;
        }

        render.update(dt);
    }
    return 0;
}