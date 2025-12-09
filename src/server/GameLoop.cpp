/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** GameLoop
*/

#include "GameLoop.hpp"
#include "engine/systems/RessourceManager.hpp"
#include <chrono>
#include <iostream>
#include <cstring>

GameLoop::GameLoop(
    SafeQueue<std::pair<int, InputState>>& receivedInputs,
    SafeQueue<PacketToSend>& packetsToSend)
    : _receivedInputs(receivedInputs)
    , _packetsToSend(packetsToSend)
{
}

GameLoop::~GameLoop()
{
    stop();
    join();
}

bool GameLoop::init()
{
    ResourceManager::getInstance().loadTexture("enemy.png", "assets/enemy.png");
    std::vector<WaveData> level = {
        {3.0f,  "enemy.png", 8,  1400, 200},
        {7.0f,  "enemy.png", 12, 1400, 100},
        {12.0f, "enemy.png", 16, 1400, 300},
    };
    _wave.loadLevel(level);
    std::cout << "[Server] GameLoop initialized\n";
    return true;
}

void GameLoop::start()
{
    if (_running) return;
    if (!init()) return;

    _running = true;
    _thread = std::thread(&GameLoop::run, this);
}

void GameLoop::stop()
{
    _running = false;
}

void GameLoop::join()
{
    if (_thread.joinable())
        _thread.join();
}

void GameLoop::run()
{
    auto lastTime = std::chrono::high_resolution_clock::now();
    double accumulator = 0.0;
    int broadcastTick = 0;
    while (_running) {
        auto now = std::chrono::high_resolution_clock::now();
        double frameTime = std::chrono::duration<double>(now - lastTime).count();
        lastTime = now;
        accumulator += frameTime;
        std::pair<int, InputState> input;
        while (_receivedInputs.pop(input)) {
            int playerId = input.first;
            const auto& in = input.second;
            auto players = _ecs.getEntitiesByComponents<PlayerController, Velocity>();
            for (Entity e : players) {
                auto* ctrl = _ecs.getComponent<PlayerController>(e);
                if (ctrl && ctrl->playerId == playerId) {
                    auto* vel = _ecs.getComponent<Velocity>(e);
                    if (vel) {
                        vel->x = vel->y = 0;
                        if (in.up)
                            vel->y -= 400;
                        if (in.down)
                            vel->y += 400;
                        if (in.left)
                            vel->x -= 400;
                        if (in.right)
                            vel->x += 400;
                        ctrl->isShooting = in.shoot;
                    }
                    break;
                }
            }
        }
        while (accumulator >= FIXED_DT) {
            _ecs.addTime(FIXED_DT);
            _movement.update(FIXED_DT);
            _collision.update(FIXED_DT);
            _wave.update(FIXED_DT);
            accumulator -= FIXED_DT;
        }
        if (++broadcastTick >= 2) {
            broadcastSnapshot();
            broadcastTick = 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void GameLoop::broadcastSnapshot()
{
    std::vector<EntityUpdate> snapshot;
    auto entities = _ecs.getEntitiesByComponents<Position>();
    for (Entity e : entities) {
        auto* p = _ecs.getComponent<Position>(e);
        if (!p)
            continue;
        EntityUpdate u{};
        u.entityId   = e;
        u.x          = p->x;
        u.y          = p->y;
        snapshot.push_back(u);
    }
    if (snapshot.empty())
        return;
    PacketToSend packet;
    packet.data.resize(snapshot.size() * sizeof(EntityUpdate));
    std::memcpy(packet.data.data(), snapshot.data(), packet.data.size());
    _packetsToSend.push(std::move(packet));
}
