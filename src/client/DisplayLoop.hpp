/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** DisplayLoop
*/

#pragma once
#include <SFML/System.hpp>
#include <atomic>
#include <thread>

class DisplayLoop {
    public:
        DisplayLoop();
        ~DisplayLoop();
        void start();
        void stop();
        void join();
    private:
        void displayLoop();
        sf::Clock _clock;
        std::thread _thread;
        std::atomic<bool> _running{false};
};
