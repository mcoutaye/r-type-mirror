/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** UdpServer
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

#define MAX_CLIENTS 4

class ClientInfo {
    public:
        sf::IpAddress   address;
        unsigned short  port;
        int             playerId;
        ClientInfo(sf::IpAddress ip, unsigned short p, int id)
        : address(ip), port(p), playerId(id) {}
};

class UdpServer {
    public:
        explicit UdpServer(unsigned short port);
        ~UdpServer();
        bool start();
        void stop();
        void join();
        SafeQueue<std::pair<int, InputState>> receivedInputs;
        SafeQueue<PacketToSend>               packetsToSend;
        bool addClient(sf::IpAddress ip, unsigned short port, int playerId = -1);
        void removeClient(int playerId);
        std::size_t getClientCount() const { return m_clientCount; }
        const std::vector<std::unique_ptr<ClientInfo>>& getClients() const { return m_clients; }
    private:
        void receiveThread();
        void sendThread();
        ClientInfo* findClient(const sf::IpAddress& ip, unsigned short port);
        sf::UdpSocket                                    m_socket;
        unsigned short                                   m_port;
        std::vector<std::unique_ptr<ClientInfo>>         m_clients;
        std::size_t                                      m_clientCount = 0;
        std::atomic<bool>                                m_running{false};
        std::thread                                      m_recvThread;
        std::thread                                      m_sendThread;
};
