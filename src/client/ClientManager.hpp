/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** ClientManager
*/

#pragma once
#include "UdpReceiver.hpp"
#include "UdpSender.hpp"
#include "PacketInterpreter.hpp"
#include "DisplayLoop.hpp"
#include <string>

namespace Ntw {
    class ClientManager {
    public:
        explicit ClientManager(const std::string& serverIp, unsigned short serverPort);
        ~ClientManager();
        void start();
        void stop();
        void join();
        void printClientInfo() const;
        UdpSender& getSender();
    private:
        std::string _serverIp;
        unsigned short _serverPort;
        UdpReceiver _receiver;
        UdpSender _sender;
        PacketInterpreter _interpreter;
        DisplayLoop _displayLoop;
    };
};
