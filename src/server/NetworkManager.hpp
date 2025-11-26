/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** NetworkManager
*/

#pragma once
#include "../../include/network/UdpReceiver.hpp"
#include "../../include/network/UdpSender.hpp"
#include "../../include/packet/ServerPacketInterpreter.hpp"
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
            ServerPacketInterpreter _interpreter;
            GameLoop          _gameLoop;
    };
};
