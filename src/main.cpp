// src/main.cpp
#include <SFML/Graphics.hpp>
#include <iostream>

// Tes includes locaux (chemin relatif depuis src/)
#include "ResourceManager.hpp"
#include "RenderSystem.hpp"
#include "InputSystem.hpp"
#include "CollisionSystem.hpp"
#include "WaveSystem.hpp"
#include "Components.hpp"   // tous tes structs (Position, Drawable, etc.)
#include "ecs.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "R-Type Engine");
    ECS ecs;

    // Charge resources
    ResourceManager::getInstance().loadTexture("ship.png", "assets/ship.png");
    ResourceManager::getInstance().loadTexture("enemy.png", "assets/enemy.png");
    // etc.

    // Crée systèmes
    RenderSystem render(ecs, window);
    InputSystem input(ecs);
    CollisionSystem collision(ecs);
    WaveSystem waves(ecs);

    // Charge level
    std::vector<WaveData> level = {
        {2.f, "enemy", 5, 800.f, 200.f},
        {10.f, "enemy", 10, 800.f, 400.f}
    };
    waves.loadLevel(level);

    // Crée joueur
    Entity player = ecs.createEntity();
    ecs.addComponent<Position>(player, {100.f, 500.f});
    ecs.addComponent<Velocity>(player, {0.f, 0.f});
    ecs.addComponent<Drawable>(player, {"ship.png", {0,0,64,64}, 10});
    ecs.addComponent<PlayerController>(player, {0});
    ecs.addComponent<Collider>(player, {50,50,true,1,25});
    ecs.addComponent<Health>(player, {100,100});

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        float dt = clock.restart().asSeconds();

        // UPDATE SYSTEMS (ordre IMPORTANT !)
        input.update(dt);           // inputs → velocity
        waves.update(dt);           // spawn
        collision.update(dt);       // collisions
        render.update(dt);          // render DERNIER

        // Mouvement (système séparé si besoin)
        auto movers = ecs.getEntitiesByComponents<Position, Velocity>();
        for (Entity e : movers) {
            auto* pos = ecs.getComponent<Position>(e);
            auto* vel = ecs.getComponent<Velocity>(e);
            pos->x += vel->x * dt;
            pos->y += vel->y * dt;
        }
    }
}