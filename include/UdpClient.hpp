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
#include "Timer.hpp"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <iostream>
#include <sstream>
#include <cstring>
#include <unordered_map>
#include <mutex>
#include <chrono>

#define SERVER_PORT 53000

class UdpClient {
    public:
        explicit UdpClient(sf::IpAddress serverIp = "127.0.0.1", unsigned short serverPort = SERVER_PORT);
        ~UdpClient();
        bool start(unsigned short localPort = 0);
        void stop();
        void join();
        int getLastPingMs() const { return _lastPingMs.load(); }
        SafeQueue<InputState>          inputsToSend;
        SafeQueue<std::vector<EntityUpdate>> receivedUpdates;
        Timer _timer;
    private:
        void sendPing();
        void pruneOldPings();
        void sendThread();
        void receiveThread();
        sf::UdpSocket           m_socket;
        sf::IpAddress           m_serverIp;
        unsigned short          m_serverPort;
        std::atomic<bool>       m_running{false};
        std::thread             m_sendThread;
        std::thread             m_recvThread;
        std::atomic<int>        _lastPingMs{-1};
        std::atomic<uint32_t>   _nextPingId{1};
        std::unordered_map<uint32_t, std::chrono::steady_clock::time_point> _pendingPings;
        std::mutex              _pingMutex;
};
