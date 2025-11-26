/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** PacketInterpreter
*/

#pragma once
#include "UdpReceiver.hpp"
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

namespace Ntw {
    class PacketInterpreter {
        public:
            explicit PacketInterpreter(UdpReceiver& receiver);
            ~PacketInterpreter();
            void start();
            void stop();
            void join();
        private:
            void interpreterLoop();
            UdpReceiver& _receiver;
            std::queue<ReceivedPacket> _packetBuffer;
            std::mutex _mutex;
            std::thread _thread;
            std::atomic<bool> _running{false};
    };
};
