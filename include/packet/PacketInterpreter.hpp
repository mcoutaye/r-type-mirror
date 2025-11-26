/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** PacketInterpreter
*/

#pragma once
#include "../network/UdpReceiver.hpp"
#include "../protocol/Protocol.hpp"
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <atomic>

namespace Ntw {
    class PacketInterpreter {
        public:
            PacketInterpreter(UdpReceiver& receiver);
            virtual ~PacketInterpreter();
            void start();
            void stop();
            void join();
        protected:
            virtual void onInputPacket(const Protocol::InputPacket& input) = 0;
            virtual void onPositionPacket(const Protocol::PositionPacket& pos) = 0;
        private:
            void interpreterLoop();
            UdpReceiver& _receiver;
            std::queue<ReceivedPacket> _packetBuffer;
            std::mutex _mutex;
            std::thread _thread;
            std::atomic<bool> _running{false};
    };
}
