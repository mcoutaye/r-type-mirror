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
        RessourceManager& _resourceManager;
    private:
        void applyUpdate(EntityUpdate &update);

        std::unordered_map<std::size_t, std::size_t> serverToClientEntityRelation;
        float _shootCooldown = 0.f;
        Entity _localPlayerEntity = -1;

        ECS _ecs;

        sf::RenderWindow _window;

        InputSystem _inputSystem{_ecs};
        RenderSystem _renderSystem{_ecs, _window, _resourceManager};
        MoveSystem _moveSystem{_ecs};
        bool _debugHitboxes = false;

        UdpClient _UDP;
};

Client::Client(sf::IpAddress serverIp)
    : _UDP(UdpClient(serverIp, SERVER_PORT)), _resourceManager(RessourceManager::getInstance()), _ecs(ECS())
{
    _window.create(sf::VideoMode(1920, 1080), "R-TYPE - CLIENT", sf::Style::Fullscreen);
    _window.setFramerateLimit(60);
    _UDP.start();
    _timer = Timer();
    _running = true;

    // Initialize resources on client side for rendering
    _resourceManager.initialize();

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
    _moveSystem.update(1.0f / 60.f);

    // ============================================================
    // KILL ENTITIES WITH NO HEALTH (client-side cleanup)
    // Server sends death notifications, but we also check locally
    // for responsiveness and to handle edge cases
    // ============================================================
    auto entitiesWithHealth = _ecs.getEntitiesByComponents<Health_t>();
    for (Entity e : entitiesWithHealth) {
        auto* health = _ecs.getComponent<Health_t>(e);
        if (health && health->current <= 0) {
            _ecs.killEntity(e);
            
            // Remove from mapping if exists
            for (auto it = serverToClientEntityRelation.begin(); it != serverToClientEntityRelation.end(); ++it) {
                if (it->second == e) {
                    serverToClientEntityRelation.erase(it);
                    break;
                }
            }
            
            // Reset local player entity reference if it was the player
            if (e == _localPlayerEntity) {
                _localPlayerEntity = -1;
            }
        }
    }

    // Remove entities out of bounds
    auto entities = _ecs.getEntitiesByComponents<Position_t>();
    for (Entity e : entities) {
        // Don't kill the local player if they go out of bounds (let server handle it or block movement)
        if (e == _localPlayerEntity) continue;

        auto* pos = _ecs.getComponent<Position_t>(e);
        if (pos && (pos->x < -100.f || pos->x > 2200.f || pos->y < -100.f || pos->y > 1200.f)) {
            // Kill stars that go off-screen to prevent memory leak
            if (!_ecs.hasComponent<Star_t>(e)) {
                _ecs.killEntity(e);
            }
        }
    }

    // Destroy stars that go off-screen to prevent accumulation and MAX_ENTITIES assertion failure
    auto stars = _ecs.getEntitiesByComponents<Star_t, Position_t>();
    for (Entity e : stars) {
        auto* pos = _ecs.getComponent<Position_t>(e);
        if (pos && pos->x < -50.f) {  // Star has left the screen
            _ecs.killEntity(e);
        }
    }
    
    // Recreate missing stars to maintain starfield count
    // This replaces old stars that went off-screen with new ones
    stars = _ecs.getEntitiesByComponents<Star_t, Position_t>();
    if (stars.size() < 150) {  // Target is 150 stars (from initializeGame)
        int starsToDeclare = 150 - stars.size();
        for (int i = 0; i < starsToDeclare; ++i) {
            float x = 1930.f;  // Spawn at right edge
            float y = static_cast<float>(rand() % 1080);
            Factory::createStar(_ecs, x, y, 10);  // Use same layer count as initializeGame
        }
    }

    // ============================================================
    // CLIENT-SIDE COLLISION: Visual prediction only
    // Remove projectiles that appear to hit targets for smoother visuals
    // Server handles actual damage and authoritative collision detection
    // ============================================================
    auto projectiles = _ecs.getEntitiesByComponents<Projectile_t, Position_t, Collider_t>();
    auto targets = _ecs.getEntitiesByComponents<Position_t, Collider_t>();

    for (Entity p : projectiles) {
        auto projPos = _ecs.getComponent<Position_t>(p);
        auto projCol = _ecs.getComponent<Collider_t>(p);

        if (!projPos || !projCol) continue;

        for (Entity t : targets) {
            if (p == t) continue;
            if (t == _localPlayerEntity) continue;  // Don't predict hits on local player
            if (_ecs.hasComponent<Projectile_t>(t)) continue;  // Don't check projectile vs projectile

            auto targetPos = _ecs.getComponent<Position_t>(t);
            auto targetCol = _ecs.getComponent<Collider_t>(t);

            if (!targetPos || !targetCol) continue;

            // Center-based AABB check to match server CollisionSystem
            float left1 = projPos->x - projCol->width / 2.f;
            float right1 = projPos->x + projCol->width / 2.f;
            float top1 = projPos->y - projCol->height / 2.f;
            float bottom1 = projPos->y + projCol->height / 2.f;

            float left2 = targetPos->x - targetCol->width / 2.f;
            float right2 = targetPos->x + targetCol->width / 2.f;
            float top2 = targetPos->y - targetCol->height / 2.f;
            float bottom2 = targetPos->y + targetCol->height / 2.f;

            if (left1 < right2 && right1 > left2 &&
                top1 < bottom2 && bottom1 > top2) {

                // Visual prediction: remove projectile immediately for smooth gameplay
                // Server will send authoritative update about actual hit
                _ecs.killEntity(p);
                break;
            }
        }
    }
}

void Client::applyUpdate(EntityUpdate &update)
{
    Entity entity = 0;

    if (serverToClientEntityRelation.find(update.entityId) == serverToClientEntityRelation.end()) {
        // Entity does not exist for client, create it
        entity = _ecs.createEntity();
        serverToClientEntityRelation[update.entityId] = entity;

        // Determine texture name based on entity type
        std::string textureName = "ship";  // Default to player
        if (update.entityType == 1) {      // Enemy
            textureName = "enemy";
        } else if (update.entityType == 2) {  // Projectile
            textureName = "bullet";
        } else if (update.entityType == 3) {  // Destructible tile
            textureName = "block";
        }

        RessourceManager& rm = RessourceManager::getInstance();
        sf::IntRect spriteRect = rm.getSpriteRect(textureName);

        // Scale: shrink tiles visually; other entities keep current normalization
        float scale = 0.5f;            // Default for players/enemies/projectiles
        if (update.entityType == 3) {
            scale = 0.5f;             // Destructible tiles scaled down (was 1.0)
        }

        // Create Collider based on entity type - MUST match server-side dimensions
        _ecs.addComponents<Position_t, Health_t, Drawable_t, Collider_t>(entity,
            {update.position.x, update.position.y},
            {update.health.current, update.health.max},
            Drawable_t{textureName, {spriteRect}, 0, 0.1f, 0.0f, true, 10, true, scale, 0.f},
            (update.entityType == 0) ? Factory::createPlayerCollider() :
            (update.entityType == 1) ? Factory::createEnemyCollider() :
            (update.entityType == 2) ? Factory::createProjectileCollider(2, 0) :
            Factory::createTileCollider());

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

        if (health->current > health->max) {
            health->current = health->max;
        }

    }

    if (update.tick == MAGIC_TICK_LOCAL_PLAYER) {
        _localPlayerEntity = entity;
    } else if (update.tick == MAGIC_TICK_DEATH_OTHER || update.tick == MAGIC_TICK_DEATH_PLAYER) {
        // Entity died
        _ecs.killEntity(entity);
        if (serverToClientEntityRelation.find(update.entityId) != serverToClientEntityRelation.end()) {
            serverToClientEntityRelation.erase(update.entityId);
        }
        if (update.tick == MAGIC_TICK_DEATH_PLAYER) {
            // Played killed it, maybe play sound or add score
            std::cout << "I killed entity " << update.entityId << "!" << std::endl;
        }
    } else if (update.tick == MAGIC_TICK_SHOOT_PLAYER || update.tick == MAGIC_TICK_SHOOT_ENEMY) {
        if (entity != _localPlayerEntity) {
            if (update.tick == MAGIC_TICK_SHOOT_PLAYER) {
                Factory::createProjectile(_ecs, update.position.x + 64.f, update.position.y + 20.f, 800.f, 0.f, 1, 25, "bullet");
            } else {
                Factory::createProjectile(_ecs, update.position.x - 20.f, update.position.y + 20.f, -800.f, 0.f, 2, 25, "bullet");
            }
        }
    }
}

void Client::render()
{
    if (_debugHitboxes) _renderSystem.debugON(); else _renderSystem.debugOFF();
    _renderSystem.update(0); // dt is not used in render system
}

void Client::processInput()
{
    InputState inputs = {0, 0, 0, 0, 0, 0};
    inputs.tick = _timer.getCurrentFrame();
    _inputSystem.update(0);

    // Client-only toggle for collider debug
    if (_inputSystem.wasActionPressed(GameAction::ToggleDebug)) {
        _debugHitboxes = !_debugHitboxes;
    }

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
                     Factory::createProjectile(_ecs, pos->x + 64.f, pos->y + 20.f, 800.f, 0.f, 1, 25, "bullet");
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
