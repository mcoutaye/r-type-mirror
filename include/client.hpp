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
    #include "engine/systems/MoveSystem.hpp"
    #include "engine/systems/RessourceManager.hpp"
    #include "engine/StageFactory.hpp"
    #include "engine/EntityFactory.hpp"
    #include "engine/systems/SoundSystem.hpp"
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
        float _shootCooldown = 0.f;
        Entity _localPlayerEntity = -1;

        ResourceManager& _resourceManager;
        ECS _ecs;

        sf::RenderWindow _window;

        InputSystem _inputSystem{_ecs};
        RenderSystem _renderSystem{_ecs, _window, _resourceManager};
        MoveSystem _moveSystem{_ecs};
        SoundSystem _soundSystem{_ecs};

        UdpClient _UDP;
};

Client::Client(sf::IpAddress serverIp)
    : _UDP(UdpClient(serverIp, SERVER_PORT)),
      _resourceManager(ResourceManager::getInstance()),
      _ecs(ECS())
{
    _window.create(sf::VideoMode(1920, 1080), "R-TYPE - CLIENT", sf::Style::Fullscreen);
    _window.setFramerateLimit(60);
    _UDP.start();
    _timer = Timer();
    _running = true;

    // === CRÉATION DU STAGE ===
    Factory::createStarfield(_ecs, 150, 1920.f, 1080.f, 10);

    // === CHARGEMENT DES SONS ===
    _soundSystem.loadSound("shoot.ogg", "assets/sound/shoot.ogg");
    _soundSystem.loadSound("enemy_death.ogg", "assets/sound/enemy_death.ogg");
    _soundSystem.loadSound("player_death.ogg", "assets/sound/player_death.ogg");
    _soundSystem.loadSound("block_destruction.ogg", "assets/sound/block_destruction.ogg");
    _soundSystem.loadSound("hit.ogg", "assets/sound/hit.ogg");

    // Musique de fond (optionnel)
    // Tu peux créer une entité avec BackgroundMusic_t plus tard, ou ici :
    Entity music = _ecs.createEntity();
    _ecs.addComponent(music, BackgroundMusic_t{"assets/music/background.ogg", true, 40.f});
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
    _moveSystem.update(1.0f / 60.f);

    // Remove entities out of bounds
    auto entities = _ecs.getEntitiesByComponents<Position_t>();
    for (Entity e : entities) {
        // Don't kill the local player if they go out of bounds (let server handle it or block movement)
        if (e == _localPlayerEntity) continue;

        auto* pos = _ecs.getComponent<Position_t>(e);
        if (pos && (pos->x < -100.f || pos->x > 2200.f || pos->y < -100.f || pos->y > 1200.f)) {
             _ecs.killEntity(e);
        }
    }

    // Client side collision to remove projectiles
    auto projectiles = _ecs.getEntitiesByComponents<Projectile_t, Position_t, Collider_t>();
    auto targets = _ecs.getEntitiesByComponents<Position_t, Collider_t>();

    for (Entity p : projectiles) {
        auto projPos = _ecs.getComponent<Position_t>(p);
        auto projCol = _ecs.getComponent<Collider_t>(p);

        if (!projPos || !projCol) continue;

        for (Entity t : targets) {
            if (p == t) continue;
            if (t == _localPlayerEntity) continue;
            if (_ecs.hasComponent<Projectile_t>(t)) continue;

            auto targetPos = _ecs.getComponent<Position_t>(t);
            auto targetCol = _ecs.getComponent<Collider_t>(t);

            if (!targetPos || !targetCol) continue;

            if (projPos->x < targetPos->x + targetCol->width &&
                projPos->x + projCol->width > targetPos->x &&
                projPos->y < targetPos->y + targetCol->height &&
                projPos->y + projCol->height > targetPos->y) {

                _ecs.killEntity(p);
                break;
            }
        }
    }
}

void Client::applyUpdate(EntityUpdate &update)
{
    Entity entity = 0;

    _timer.updateClock();
    std::vector<EntityUpdate> updates;
    while (_UDP.receivedUpdates.tryPop(updates)) {
        for (auto &update : updates)
            this->applyUpdate(update);
    }

    _moveSystem.update(1.0f / 60.f);
    _soundSystem.update(1.0f / 60.f);

    if (serverToClientEntityRelation.find(update.entityId) == serverToClientEntityRelation.end()) {
        // Entity does not exist for client, create it
        entity = _ecs.createEntity();
        serverToClientEntityRelation[update.entityId] = entity;

        // Suppose entity is Player, but can be everything else.
        // should add EntityType to EntityUpdate later.
        _ecs.addComponents<Position_t, Health_t, Drawable_t, Collider_t>(entity,
            {update.position.x, update.position.y},
            {update.health.current, update.health.max},
            {"ship.png", sf::IntRect(0, 0, 64, 64), 10, true, 1.f, 0.f},
            Collider_t{64.f, 64.f, true, 2, 0});

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

    if (update.tick == MAGIC_TICK_LOCAL_PLAYER) {
        _localPlayerEntity = entity;
    } else if (update.tick == MAGIC_TICK_DEATH_OTHER || update.tick == MAGIC_TICK_DEATH_PLAYER) {
        // Entity died
        _ecs.killEntity(entity);
        // Jouer un son d'explosion selon le type (approximation simple)
        if (_ecs.hasComponent<Projectile_t>(entity)) {
            // C'était un projectile → petit hit
            Entity hitSound = _ecs.createEntity();
            _ecs.addComponent(hitSound, Factory::createSound("hit.ogg", 70.f));
        } else {
            // Ennemi ou joueur → explosion
            Entity explosionSound = _ecs.createEntity();
            _ecs.addComponent(explosionSound, Factory::createSound("enemy_death.ogg", 90.f));
        }

        if (serverToClientEntityRelation.find(update.entityId) != serverToClientEntityRelation.end()) {
            serverToClientEntityRelation.erase(update.entityId);
        }

        if (update.tick == MAGIC_TICK_DEATH_PLAYER) {
            std::cout << "I killed entity " << update.entityId << "!" << std::endl;
        }
    } else if (update.tick == MAGIC_TICK_SHOOT_PLAYER || update.tick == MAGIC_TICK_SHOOT_ENEMY) {
        if (entity != _localPlayerEntity) {  // On ne joue pas le son si c'est notre propre tir (déjà joué localement)
            if (update.tick == MAGIC_TICK_SHOOT_PLAYER) {
                Factory::createProjectile(_ecs,
                    update.position.x + 64.f, update.position.y + 20.f,
                    800.f, 0.f, 1, 25, "shoot.png",
                    -1, "shoot.ogg");
            } else {
                Factory::createProjectile(_ecs,
                    update.position.x - 20.f, update.position.y + 20.f,
                    -800.f, 0.f, 2, 25, "shoot.png",
                    -1, "shoot.ogg");
            }
        }
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
    if (_inputSystem.isActionActive(GameAction::Shoot)) {
        inputs.shoot = 1;
        if (_shootCooldown <= 0.f) {
            if (_localPlayerEntity != -1) {
                auto pos = _ecs.getComponent<Position_t>(_localPlayerEntity);
                if (pos) {
                    // Création du projectile + son
                    Factory::createProjectile(_ecs,
                        pos->x + 64.f, pos->y + 20.f,
                        800.f, 0.f,
                        1, 25, "shoot.png",
                        -1, "shoot.ogg");  // <-- le son est ajouté via la factory

                    _shootCooldown = SHOOT_DELAY;
                }
            }
        }
    }
    if (_shootCooldown > 0.f)
        _shootCooldown -= 1.0f / 60.f;

    if (_inputSystem.isActionActive(GameAction::Quit)) {
        inputs.tick = MAGIC_TICK_CLIENT_QUIT;
        _running = false;
    }
    _UDP.inputsToSend.push(inputs);
}


#endif /* !CLIENT_HPP_ */
