/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** GameLoop
*/

// src/server/ServerGameLoop.hpp
#pragma once
#include "engine/ecs/ecs.hpp"
#include "engine/systems/MovementSystem.hpp"
#include "engine/systems/CollisionSystem.hpp"
#include "engine/systems/WaveSystem.hpp"
#include "SafeQueue.hpp"
#include "NetworkProtocol.hpp"
#include <thread>
#include <atomic>
#include <vector>

class GameLoop {
    public:
        GameLoop(
            SafeQueue<std::pair<int, InputState>>& receivedInputs,
            SafeQueue<PacketToSend>& packetsToSend);

        ~GameLoop();
        bool init();
        void start();   // lance le thread
        void stop();    // demande l'arrêt
        void join();    // attend la fin
    private:
        void run();
        void fixedUpdate(double dt);
        void broadcastSnapshot();
        ECS _ecs;
        MovementSystem  _movement{_ecs};
        CollisionSystem _collision{_ecs};
        WaveSystem      _wave{_ecs};
        SafeQueue<std::pair<int, InputState>>& _receivedInputs;
        SafeQueue<PacketToSend>&               _packetsToSend;
        std::thread _thread;
        std::atomic<bool> _running{false};
        static constexpr double FIXED_DT = 1.0 / 60.0;
};
