/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** client
*/

#ifndef CLIENT_HPP_
    #define CLIENT_HPP_

    #include "ecs.hpp"
    #include "UdpClient.hpp"
    #include "Timer.hpp"
    #include "engine/systems/Components.hpp"
    #include "engine/systems/InputSystem.hpp"
    #include "engine/systems/RenderSystem.hpp"
    #include "engine/systems/RessourceManager.hpp"
    #include "engine/StageFactory.hpp"
    #include "engine/EntityFactory.hpp"
    #include "serializer.hpp"

class Client {
    public:
        Client(sf::IpAddress serverIp = "127.0.0.1");
        ~Client();

        void update(); // Process all recieved updates from server
        void render(); // Render all entities from ECS
        void processInput(); // Retrieve and send inputs to server

        Timer _timer;
        bool _running;
    private:
        void applyUpdate(EntityUpdate &update);

        std::unordered_map<std::size_t, std::size_t> serverToClientEntityRelation;

        ResourceManager& _resourceManager;
        ECS _ecs;

        sf::RenderWindow _window;

        InputSystem _inputSystem{_ecs};
        RenderSystem _renderSystem{_ecs, _window, _resourceManager};

        UdpClient _UDP;
};

Client::Client(sf::IpAddress serverIp)
    : _UDP(UdpClient(serverIp, SERVER_PORT)), _resourceManager(ResourceManager::getInstance()), _ecs(ECS())
{
    _window.create(sf::VideoMode(1920, 1080), "R-TYPE - CLIENT", sf::Style::Fullscreen);
    _window.setFramerateLimit(60);
    _UDP.start();
    _timer = Timer();
    _running = true;

        // === CRÉATION DU STAGE ===
    Factory::createStarfield(_ecs, 150, 1920.f, 1080.f, 10);
}

Client::~Client() {}

void Client::update()
{
    _timer.updateClock();
    std::vector<EntityUpdate> updates;

    while (_UDP.receivedUpdates.tryPop(updates)) {
        for (auto &update : updates)
            this->applyUpdate(update);
    }
}

void Client::applyUpdate(EntityUpdate &update)
{
    Entity entity = 0;

    if (serverToClientEntityRelation.find(update.entityId) == serverToClientEntityRelation.end()) {
        // Entity does not exist for client, create it
        entity = _ecs.createEntity();
        serverToClientEntityRelation[update.entityId] = entity;

        // Suppose entity is Player, but can be everything else.
        // should add EntityType to EntityUpdate later.
        _ecs.addComponents<Position_t, Health_t, Drawable_t>(entity,
            {update.position.x, update.position.y},
            {update.health.current, update.health.max},
            {"ship.png", sf::IntRect(0, 0, 64, 64), 10, true, 1.f, 0.f});

    } else {
        entity = serverToClientEntityRelation[update.entityId];
    }

    auto pos = _ecs.getComponent<Position_t>(entity);
    auto health = _ecs.getComponent<Health_t>(entity);

    if (pos) {
        pos->x = update.position.x;
        pos->y = update.position.y;
    }
    if (health) {
        health->current = update.health.current;
        health->max = update.health.max;
    }
}

void Client::render()
{
    _renderSystem.update(0); // dt is not used in render system
}

void Client::processInput()
{
    InputState inputs = {0, 0, 0, 0, 0, 0};
    inputs.tick = _timer.getCurrentFrame();
    _inputSystem.update(0);
    if (_inputSystem.isActionActive(GameAction::MoveUp))
        inputs.up = 1;
    if (_inputSystem.isActionActive(GameAction::MoveDown))
        inputs.down = 1;
    if (_inputSystem.isActionActive(GameAction::MoveLeft))
        inputs.left = 1;
    if (_inputSystem.isActionActive(GameAction::MoveRight))
        inputs.right = 1;
    if (_inputSystem.isActionActive(GameAction::Shoot))
        inputs.shoot = 1;
    if (_inputSystem.isActionActive(GameAction::Quit))
        _running = false;
    _UDP.inputsToSend.push(inputs);
}


#endif /* !CLIENT_HPP_ */
