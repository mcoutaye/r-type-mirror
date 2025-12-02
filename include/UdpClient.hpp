/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** UdpClient
*/

#pragma once
#include "SafeQueue.hpp"
#include <SFML/Network.hpp>
#include "NetworkProtocol.hpp"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <iostream>
#include <sstream>
#include <cstring>

#define SERVER_PORT 53000

class UdpClient {
    public:
        explicit UdpClient(sf::IpAddress serverIp = "127.0.0.1", unsigned short serverPort = SERVER_PORT);
        ~UdpClient();
        bool start(unsigned short localPort = 0);
        void stop();
        void join();
        SafeQueue<InputState>          inputsToSend;
        SafeQueue<std::vector<EntityUpdate>> receivedUpdates;
    private:
        void sendThread();
        void receiveThread();
        sf::UdpSocket           m_socket;
        sf::IpAddress           m_serverIp;
        unsigned short          m_serverPort;
        std::atomic<bool>       m_running{false};
        std::thread             m_sendThread;
        std::thread             m_recvThread;
};
