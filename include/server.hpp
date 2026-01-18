/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** server
*/

#ifndef SERVER_HPP_
    #define SERVER_HPP_

    #include "UdpServer.hpp"
    #include "Timer.hpp"
    #include "ecs.hpp"
    #include "engine/systems/Components.hpp"
    #include "engine/systems/MoveSystem.hpp"
    #include "engine/systems/MovementSystem.hpp"
    #include "engine/systems/WaveSystem.hpp"
    #include "engine/systems/CollisionSystem.hpp"
    #include "serializer.hpp"
    #include "engine/EntityFactory.hpp"
    // StageFactory no longer used server-side for obstacles/tiles
    #include "engine/systems/RessourceManager.hpp"

#define FRAME 60000

class Server {
    public:
        explicit Server(unsigned short port);
        ~Server();

        void processInputs(); // Change position with recieved inputs
        void update();        // Updates ECS with systems
        void broadcast();     // Send updates to all clients

        Timer _timer;
        bool _running;
    private:
        void applyInput(std::pair<int, InputState> &currentItem);
        void entityForClients(); // Create ECS Entity if a client connect itself

        uint16_t _nbPlayer;
        std::unordered_map<std::size_t, std::size_t> clientToPlayerRelation;

        WaveSystem _waveSystem{_ecs};
        MoveSystem _movSys{_ecs};
        MovementSystem _movementSystem{_ecs};
        CollisionSystem _collisionSystem{_ecs};

        UdpServer _UDP;
        ECS _ecs;
};

Server::Server(unsigned short port)
    : _UDP(UdpServer(port)), _ecs(ECS())
{
    _UDP.start();
    _timer = Timer();
    _running = true;
    _nbPlayer = 0;

    // Initialize resources to ensure sprite rects are available server-side
    RessourceManager::getInstance().initialize();

    // Factory::createScreenBorders(_ecs, 1920.f, 1080.f, 5.f);

    std::vector<WaveData_t> level = {
        {3.0f,  "enemy", 6,  2000.f, 200.f},
        {8.0f,  "enemy", 10, 2000.f, 400.f},
        {15.0f, "enemy", 15, 2000.f, 600.f}
    };
    _waveSystem.loadLevel(level);

    // ============================================================
    // CREATE DESTRUCTIBLE TILES (OBSTACLES)
    // ============================================================
    Factory::createDestructibleTile(_ecs, 400.f, 200.f, 150, "block");
    Factory::createDestructibleTile(_ecs, 400.f, 500.f, 150, "block");
    Factory::createDestructibleTile(_ecs, 400.f, 800.f, 150, "block");
    Factory::createDestructibleTile(_ecs, 900.f, 250.f, 100, "block");
    Factory::createDestructibleTile(_ecs, 900.f, 540.f, 100, "block");
    Factory::createDestructibleTile(_ecs, 900.f, 830.f, 100, "block");
    Factory::createDestructibleTile(_ecs, 1400.f, 350.f, 120, "block");
    Factory::createDestructibleTile(_ecs, 1400.f, 700.f, 120, "block");
}

Server::~Server() {}

void Server::applyInput(std::pair<int, InputState> &currentItem)
{
    if (clientToPlayerRelation.find(currentItem.first) == clientToPlayerRelation.end())
        return;

    if (currentItem.second.tick == MAGIC_TICK_CLIENT_QUIT) {
        Entity e = clientToPlayerRelation[currentItem.first];
        auto health = _ecs.getComponent<Health_t>(e);
        if (health) {
            health->current = 0;
            health->lastAttackerId = -1;
        }
        clientToPlayerRelation.erase(currentItem.first);
        _nbPlayer--;
        std::cout << "Client " << currentItem.first << " disconnected." << std::endl;
        return;
    }

    Entity e = clientToPlayerRelation[currentItem.first];
    auto velo = _ecs.getComponent<Velocity_t>(e);

    if (!velo)
        return;

    float speed = 400.0f;
    velo->x = 0;
    velo->y = 0;

    if (currentItem.second.right)
        velo->x = speed;
    if (currentItem.second.left)
        velo->x = -speed;
    if (currentItem.second.up)
        velo->y = -speed;
    if (currentItem.second.down)
        velo->y = speed;

    auto ctrl = _ecs.getComponent<PlayerController_t>(e);
    if (ctrl) {
        ctrl->isShooting = currentItem.second.shoot;
    }
}

void Server::processInputs()
{
    // Reset velocity for all players to prevent sliding
    for (auto& pair : clientToPlayerRelation) {
        auto velo = _ecs.getComponent<Velocity_t>(pair.second);
        if (velo) {
            velo->x = 0;
            velo->y = 0;
        }
    }

    this->entityForClients();
    std::pair<int, InputState> first;
    std::pair<int, InputState> input;

    if (!_UDP.receivedInputs.tryPop(first))
        return; // No input
    input = first;

    while (first.second.tick <= input.second.tick) {
        this->applyInput(input);
        // Do the next arg
        if (!_UDP.receivedInputs.tryPop(input))
            return; // No more inputs
    }

    // still process the last input, you cant put it back in the queue anyways
    this->applyInput(input);
}

void Server::update()
{
    _movSys.update(1.0f / 60.f); // Update with fixed delta time
    _movementSystem.update(1.0f / 60.f);
    _waveSystem.update(1.0f / 60.f);

    // Remove entities out of bounds
    auto entities = _ecs.getEntitiesByComponents<Position_t>();
    for (Entity e : entities) {
        if (_ecs.hasComponent<PlayerController_t>(e))
            continue;

        auto* pos = _ecs.getComponent<Position_t>(e);
        if (pos->x < -100.f || pos->x > 2200.f || pos->y < -100.f || pos->y > 1200.f) {
             _ecs.killEntity(e);
        }
    }

    // Shooting logic for players
    for (Entity e : _ecs.getEntitiesByComponents<PlayerController_t, Position_t>()) {
        auto* ctrl = _ecs.getComponent<PlayerController_t>(e);
        auto* pos = _ecs.getComponent<Position_t>(e);

        if (ctrl && ctrl->isShooting && ctrl->shootCooldown <= 0.f) {
            Factory::createProjectile(_ecs, pos->x + 64.f, pos->y + 20.f, 800.f, 0.f, 1, 50, "bullet", e);
            ctrl->shootCooldown = SHOOT_DELAY;
            _ecs.addComponent<JustShot_t>(e, {true});
        }
        ctrl->shootCooldown -= 1.0f / 60.f;
    }

    // ============================================================
    // COLLISION DETECTION - Server authoritative using CollisionSystem
    // Handles: projectile hits, contact damage, entity death
    // ============================================================
    _collisionSystem.update(1.0f / 60.f);
}

void Server::broadcast()
{
    // send every player entities or projectile for the moment
    auto entities = _ecs.getEntitiesByComponents<Position_t, Health_t, SendUpdate_t>(); // Players & Enemies
    auto clients = _UDP.getClients();
    uint16_t serverTick = _timer.getCurrentFrame();

    for (Entity e : entities) {
        auto pos = _ecs.getConstComponent<Position_t>(e);
        auto pv = _ecs.getConstComponent<Health_t>(e);

        // Check if this entity belongs to a client
        int ownerClientId = -1;
        for (const auto& pair : clientToPlayerRelation) {
            if (pair.second == e) {
                ownerClientId = pair.first;
                break;
            }
        }

        for (const auto& client : clients) {
            uint16_t tickToSend = serverTick;
            if (client.playerId == ownerClientId) {
                tickToSend = MAGIC_TICK_LOCAL_PLAYER; // Magic value for "This is you"
            }

            if (_ecs.hasComponent<JustShot_t>(e)) {
                if (_ecs.hasComponent<PlayerController_t>(e)) {
                    tickToSend = MAGIC_TICK_SHOOT_PLAYER;
                } else {
                    tickToSend = MAGIC_TICK_SHOOT_ENEMY;
                }
            }

            // Handle death events
            if (pv->current <= 0) {
                // Check if this client killed the entity
                // We need to find the player ID associated with the lastAttackerId entity
                int attackerClientId = -1;
                for (const auto& pair : clientToPlayerRelation) {
                    if (pair.second == pv->lastAttackerId) {
                        attackerClientId = pair.first;
                        break;
                    }
                }

                if (client.playerId == attackerClientId) {
                    tickToSend = MAGIC_TICK_DEATH_PLAYER; // You killed it
                } else {
                    tickToSend = MAGIC_TICK_DEATH_OTHER; // It died
                }
            }

            // total size needed (Entity ID + tick + entityType + Position + Health (current + max))
            // Note: We manually serialize health to exclude lastAttackerId and match EntityUpdate struct
            size_t totalSize = sizeof(Entity) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(Position_t) + (2 * sizeof(int));
            std::vector<uint8_t> packetData(totalSize);
            size_t offset = 0;

            std::memcpy(packetData.data() + offset, &e, sizeof(Entity));
            offset += sizeof(Entity);
            std::memcpy(packetData.data() + offset, &tickToSend, sizeof(uint16_t));
            offset += sizeof(uint16_t);
            
            // Determine entity type: 0=player, 1=enemy, 2=projectile, 3=destructible tile
            uint8_t entityType = 0; // Default to player
            if (_ecs.hasComponent<PlayerController_t>(e)) {
                entityType = 0; // Player
            } else if (_ecs.hasComponent<MovementPattern_t>(e)) {
                entityType = 1; // Enemy (has movement pattern)
            } else if (_ecs.hasComponent<Projectile_t>(e)) {
                entityType = 2; // Projectile
            } else if (_ecs.hasComponent<DestructibleTile_t>(e)) {
                entityType = 3; // Destructible tile
            }
            std::memcpy(packetData.data() + offset, &entityType, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            
            std::memcpy(packetData.data() + offset, pos, sizeof(Position_t));
            offset += sizeof(Position_t);

            // Manual Health serialization
            int currentHealth = pv->current;
            int maxHealth = pv->max;
            std::memcpy(packetData.data() + offset, &currentHealth, sizeof(int));
            offset += sizeof(int);
            std::memcpy(packetData.data() + offset, &maxHealth, sizeof(int));

            _UDP.packetsToSend.push({client.address, client.port, packetData});
        }

        if (pv->current <= 0) {
             std::cout << "Entity " << e << " died (Killer: " << pv->lastAttackerId << ")" << std::endl;
        }
    }

    // Cleanup dead entities after broadcast
    for (Entity e : entities) {
        auto pv = _ecs.getConstComponent<Health_t>(e);
        if (pv->current <= 0) {
            _ecs.killEntity(e);
        }
        if (_ecs.hasComponent<JustShot_t>(e)) {
            _ecs.killComponent<JustShot_t>(e);
        }
    }
}

    // auto projectiles = _ecs.getEntitiesByComponents<Projectile_t>();

    // for (Entity projectile : projectiles) {
    //     auto comp =  _ecs.getConstComponent<Projectile_t>(projectile);
    //     std::vector<uint8_t> packetData(sizeof(Projectile_t));
    //     std::memcpy(packetData.data(), &comp, sizeof(Projectile_t));
    //     std::queue<PacketToSend> queue = _UDP.createEveryonePacket(packetData);

    //     while (!queue.empty()) {
    //         _UDP.packetsToSend.push(queue.front());
    //         queue.pop();
    //     }
    // }

void Server::entityForClients()
{
    std::size_t clientCount = _UDP.getClientCount();

    if (clientCount == this->_nbPlayer)
        return;

    std::vector<ClientInfo> Clients = _UDP.getClients();

    for (const auto &client : Clients) {
        if (clientToPlayerRelation.find(client.playerId) == clientToPlayerRelation.end()) {
            Entity newEntity = Factory::createPlayer(_ecs, 0.f, 0.f, client.playerId, "player");
            clientToPlayerRelation.insert({ client.playerId, newEntity });
            this->_nbPlayer++;
        }
    }
}

// SafeQueue<std::pair<int, InputState>> &Server::getRecievedInput()
// {
//     return _UDP.receivedInputs;
// }

#endif /* !SERVER_HPP_ */
