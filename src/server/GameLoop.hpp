/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** GameLoop
*/

#pragma once
#include <SFML/System.hpp>
#include <atomic>
#include "../../include/network/UdpSender.hpp"
#include <thread>

class GameLoop {
    public:
        GameLoop(Ntw::UdpSender &Sender, int port, sf::IpAddress ip);
        ~GameLoop();
        void start();
        void stop();
        void join();
    private:
        void gameLoop();
        sf::Clock _clock;
        std::thread _thread;
        std::atomic<bool> _running{false};
        Ntw::UdpSender &_sender;
        int _port;
        sf::IpAddress _ip;
};
