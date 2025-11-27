/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** GameLoop
*/

#pragma once
#include <SFML/System.hpp>
#include <atomic>
#include <thread>
#include <functional>
#include "../network/UdpSender.hpp"

class GameLoop {
    public:
        using OnPositionUpdate = std::function<void(uint8_t entityId, float x, float y, float angle)>;
        GameLoop(Ntw::UdpSender& sender);
        virtual ~GameLoop();
        void start();
        void stop();
        void join();
        void setOnPositionUpdate(OnPositionUpdate callback);
    protected:
        virtual void gameLoop() = 0;
        Ntw::UdpSender& _sender;
        std::atomic<bool> _running{false};
        std::thread _thread;
        sf::Clock _clock;
        OnPositionUpdate _onPositionUpdate;
};

