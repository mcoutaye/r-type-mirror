/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** UdpReceiver
*/

#pragma once
#include <SFML/Network.hpp>
#include <queue>
#include <thread>
#include <mutex>

namespace Ntw {
    class ReceivedPacket {
        public:
            ReceivedPacket(sf::IpAddress sender, unsigned short port, std::vector<char> data);
            ReceivedPacket();
            ~ReceivedPacket() = default;
            sf::IpAddress _sender;
            unsigned short _port;
            std::vector<char> _data;
    };

    class UdpReceiver {
        public:
            explicit UdpReceiver(unsigned short port);
            ~UdpReceiver();
            void start();
            void stop();
            void join();
            bool getPacket(ReceivedPacket& packet);
            unsigned short getPort() const;
        private:
            void receiveLoop();
            sf::UdpSocket _socket;
            unsigned short _port;
            std::queue<std::unique_ptr<ReceivedPacket>> _packetQueue;
            std::mutex _mutex;
            std::thread _thread;
            bool _running = true;
    };
};