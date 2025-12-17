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
    #include "serializer.hpp"

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
}

Server::~Server() {}

void Server::applyInput(std::pair<int, InputState> &currentItem)
{
    if (clientToPlayerRelation.find(currentItem.first) == clientToPlayerRelation.end())
        return;

    Entity e = clientToPlayerRelation[currentItem.first];
    auto velo = _ecs.getComponent<Velocity_t>(e);

    if (!velo)
        return;
    if (currentItem.second.right)
        velo->x += 10;
    if (currentItem.second.left)
        velo->x -= 10;
    if (currentItem.second.up)
        velo->y += 10;
    if (currentItem.second.down)
        velo->y -= 10;
}

void Server::processInputs()
{
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
    MoveSystem movSys(this->_ecs);
    movSys.update(1.0f / 60.0f); // Update with fixed delta time
}

void Server::broadcast()
{
    // send every player entities or projectile for the moment
    // use HasChanged component later
    auto entities = _ecs.getEntitiesByComponents<Position_t, Health_t>(); // Players & Enemies

    for (Entity e : entities) {
        auto pos = _ecs.getConstComponent<Position_t>(e);
        auto pv = _ecs.getConstComponent<Health_t>(e);
        auto tick = _timer.getCurrentFrame();

        // total size needed (Entity ID + Position + Health)
        size_t totalSize = sizeof(Entity) + sizeof(uint16_t) + sizeof(Position_t) + sizeof(Health_t);
        std::vector<uint8_t> packetData(totalSize);
        size_t offset = 0;

        std::memcpy(packetData.data() + offset, &e, sizeof(Entity));
        offset += sizeof(Entity);
        std::memcpy(packetData.data() + offset, &tick, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        std::memcpy(packetData.data() + offset, pos, sizeof(Position_t));
        offset += sizeof(Position_t);
        std::memcpy(packetData.data() + offset, pv, sizeof(Health_t));

        std::queue<PacketToSend> queue = _UDP.createEveryonePacket(packetData);

        std::cout << "Entity " << e << " at {" << pos->x << ", " << pos->y
            << "} with " << pv->current << " pv" << std::endl;

        while (!queue.empty()) {
            _UDP.packetsToSend.push(queue.front());
            queue.pop();
        }
    }

    auto projectiles = _ecs.getEntitiesByComponents<Projectile_t>();

    for (Entity projectile : projectiles) {
        auto comp =  _ecs.getConstComponent<Projectile_t>(projectile);
        std::vector<uint8_t> packetData(sizeof(Projectile_t));
        std::memcpy(packetData.data(), &comp, sizeof(Projectile_t));
        std::queue<PacketToSend> queue = _UDP.createEveryonePacket(packetData);

        while (!queue.empty()) {
            _UDP.packetsToSend.push(queue.front());
            queue.pop();
        }
    }

}

void Server::entityForClients()
{
    std::size_t clientCount = _UDP.getClientCount();

    if (clientCount == this->_nbPlayer)
        return;

    std::vector<ClientInfo> Clients = _UDP.getClients();

    for (const auto &client : Clients) {
        if (clientToPlayerRelation.find(client.playerId) == clientToPlayerRelation.end()) {
            auto newEntity = _ecs.createEntity();
            _ecs.addComponents<Position_t, Health_t, Velocity_t>(newEntity, {0.f, 0.f}, {100, 100}, {0.f, 0.f});
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
