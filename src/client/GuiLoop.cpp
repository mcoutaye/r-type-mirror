/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** GuiLoop
*/

#include "GuiLoop.hpp"
#include "engine/systems/RessourceManager.hpp"
#include <iostream>

GuiLoop::GuiLoop(
    SafeQueue<InputState>& inputsToSend,
    SafeQueue<std::vector<EntityUpdate>>& receivedUpdates)
    : _inputsToSend(inputsToSend)
    , _receivedUpdates(receivedUpdates)
{
}

GuiLoop::~GuiLoop()
{
    stop();
    join();
}

bool GuiLoop::init()
{
    ResourceManager::getInstance().loadTexture("ship.png",   "assets/ship.png");
    ResourceManager::getInstance().loadTexture("enemy.png",  "assets/enemy.png");
    ResourceManager::getInstance().loadTexture("bullet.png", "assets/bullet.png");

    _localPlayer = _ecs.createEntity();
    _ecs.addComponent(_localPlayer, Position{150, 360});
    _ecs.addComponent(_localPlayer, Velocity{0, 0});
    _ecs.addComponent(_localPlayer, PlayerController{0});
    _ecs.addComponent(_localPlayer, Drawable{"ship.png", {0,0,64,64}, 20, true});
    _ecs.addComponent(_localPlayer, Collider{50,50,true,1});

    std::cout << "[Client] GuiLoop initialized\n";
    return true;
}

void GuiLoop::start()
{
    if (_running)
        return;
    if (!init())
        return;
    _running = true;
    _thread = std::thread(&GuiLoop::run, this);
}

void GuiLoop::stop()
{
    _running = false;
}

void GuiLoop::join()
{
    if (_thread.joinable())
        _thread.join();
}

void GuiLoop::run()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "R-Type");
    window.setFramerateLimit(144);
    sf::Clock clock;
    RenderSystem render(_ecs, window);
    while (_running && window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                _running = false;
        InputState state{};
        state.up    = _input.isActionActive(GameAction::MoveUp);
        state.down  = _input.isActionActive(GameAction::MoveDown);
        state.left  = _input.isActionActive(GameAction::MoveLeft);
        state.right = _input.isActionActive(GameAction::MoveRight);
        state.shoot = _input.wasActionPressed(GameAction::Shoot);
        _inputsToSend.push(state);
        _input.update(dt);
        _movement.update(dt);
        std::vector<EntityUpdate> updates;
        while (_receivedUpdates.pop(updates))
            applyServerUpdates(updates);
        render.update(dt);
    }
}

void GuiLoop::applyServerUpdates(const std::vector<EntityUpdate>& updates)
{
    for (const auto& u : updates) {
        if (u.entityId == _localPlayer) {
            auto* pos = _ecs.getComponent<Position>(_localPlayer);
            if (pos) {
                float distSq = (pos->x - u.x)*(pos->x - u.x) + (pos->y - u.y)*(pos->y - u.y);
                if (distSq > 30*30) {
                    pos->x = u.x;
                    pos->y = u.y;
                } else {
                    pos->x = pos->x * 0.7f + u.x * 0.3f;
                    pos->y = pos->y * 0.7f + u.y * 0.3f;
                }
            }
        } else {
            auto* pos = _ecs.getComponent<Position>(u.entityId);
            if (!pos) {
                Entity e = _ecs.createEntity();
                _ecs.addComponent(e, Position{u.x, u.y});
            } else {
                pos->x = u.x;
                pos->y = u.y;
            }
        }
    }
}
