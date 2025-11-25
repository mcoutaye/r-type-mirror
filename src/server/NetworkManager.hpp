/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** NetworkManager
*/

#pragma once
#include "UdpReceiver.hpp"
#include "UdpSender.hpp"
#include "PacketInterpreter.hpp"
#include "GameLoop.hpp"

namespace Ntw {
    class NetworkManager {
        public:
            explicit NetworkManager(unsigned short listenPort = 55002);
            ~NetworkManager();
            void start();
            void stop();
            void join();
            void printServerInfo() const;
            std::vector<sf::IpAddress> getLocalIpAddresses() const;
            UdpSender& getSender();
        private:
            UdpReceiver       _receiver;
            UdpSender         _sender;
            PacketInterpreter _interpreter;
            GameLoop          _gameLoop;
    };
};