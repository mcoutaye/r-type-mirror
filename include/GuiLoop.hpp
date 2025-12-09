/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** GuiLoop
*/

#pragma once
#include "engine/ecs/ecs.hpp"
#include "engine/systems/InputSystem.hpp"
#include "engine/systems/MovementSystem.hpp"
#include "engine/systems/RenderSystem.hpp"
#include "SafeQueue.hpp"
#include "NetworkProtocol.hpp"
#include <SFML/Graphics.hpp>
#include <thread>
#include <atomic>
#include <vector>

class GuiLoop {
    public:
        GuiLoop(
            SafeQueue<InputState>& inputsToSend,
            SafeQueue<std::vector<EntityUpdate>>& receivedUpdates);
        ~GuiLoop();
        bool init();
        void start();
        void stop();
        void join();
    private:
        void run();
        void applyServerUpdates(const std::vector<EntityUpdate>& updates);
        ECS _ecs;
        InputSystem    _input{_ecs};
        MovementSystem _movement{_ecs};
        SafeQueue<InputState>& _inputsToSend;
        SafeQueue<std::vector<EntityUpdate>>& _receivedUpdates;
        Entity _localPlayer{0};
        uint32_t _inputSequence{0};
        std::thread _thread;
        std::atomic<bool> _running{false};
};
