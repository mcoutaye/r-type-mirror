/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** UdpSender
*/

#pragma once
#include <SFML/Network.hpp>
#include <vector>

namespace Ntw {
    class UdpSender {
        public:
            UdpSender();
            bool sendTo(const std::vector<char>& data, const sf::IpAddress& ip, unsigned short port);
            bool broadcast(const std::vector<char>& data, unsigned short port);
        private:
            sf::UdpSocket _socket;
    };
};
