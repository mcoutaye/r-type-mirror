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

class GameLoop {
    public:
        GameLoop();
        ~GameLoop();
        void start();
        void stop();
        void join();
    private:
        void gameLoop();
        sf::Clock _clock;
        std::thread _thread;
        std::atomic<bool> _running{false};
};
