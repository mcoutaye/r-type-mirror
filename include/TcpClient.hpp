/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** TcpClient
*/

#pragma once
#include <SFML/Network.hpp>
#include <thread>
#include <atomic>
#include <chrono>
#include <string>
#include <memory>
#include <iostream>

class TcpClient {
    public:
        explicit TcpClient(const std::string& ip = "127.0.0.1", unsigned short port = 53001);
        ~TcpClient();
        bool start();
        void stop();
        void join();
        bool isConnected() const { return _connected.load(); }
    private:
        void run();
        std::string _ip;
        unsigned short _port;
        std::unique_ptr<sf::TcpSocket> _socket;
        std::thread _thread;
        std::atomic<bool> _running{false};
        std::atomic<bool> _connected{false};
        std::chrono::steady_clock::time_point _lastPingSent;
        std::chrono::steady_clock::time_point _lastPongReceived;
        static constexpr int PING_INTERVAL_MS = 1000;
        static constexpr int TIMEOUT_SEC = 5;
};