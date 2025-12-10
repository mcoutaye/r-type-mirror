/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** TcpServer
*/

#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <memory>
#include <iostream>

class TcpServer {
    public:
        TcpServer(unsigned short port = 53001);
        ~TcpServer();
        bool start();
        void stop();
        void join();
        size_t getClientCount() const;
    private:
        void run();
        unsigned short _port;
        sf::TcpListener _listener;
        std::vector<std::unique_ptr<sf::TcpSocket>> _clients;
        std::vector<std::chrono::steady_clock::time_point> _lastPing;
        sf::SocketSelector _selector;
        std::thread _thread;
        std::atomic<bool> _running{false};
        mutable std::mutex _mutex;
        static constexpr int TIMEOUT_SEC = 5;
};
