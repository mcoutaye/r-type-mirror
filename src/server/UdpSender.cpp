/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** UdpSender
*/

#include "UdpSender.hpp"
#include <iostream>

Ntw::UdpSender::UdpSender()
{
    _socket.setBlocking(false);
}

bool Ntw::UdpSender::sendTo(const std::vector<char>& data, const sf::IpAddress& ip, unsigned short port)
{
    sf::Socket::Status status = _socket.send(data.data(), data.size(), ip, port);
    if (status != sf::Socket::Done) {
        std::cerr << "Erreur envoi vers " << ip << ":" << port << std::endl;
        return false;
    }
    return true;
}

bool Ntw::UdpSender::broadcast(const std::vector<char>& data, unsigned short port)
{
    _socket.send(data.data(), data.size(), sf::IpAddress::Broadcast, port);
    return true;
}
