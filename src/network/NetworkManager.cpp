/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** NetworkManager
*/

#include "../../include/network/NetworkManager.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <sstream>

Ntw::ClientInfo::ClientInfo(const sf::IpAddress& ip, unsigned short port)
: _ip(ip), _port(port)
{
}

const sf::IpAddress& Ntw::ClientInfo::getIp() const
{
    return _ip;
}

unsigned short Ntw::ClientInfo::getPort() const
{
    return _port;
}

void Ntw::ClientInfo::setIp(const sf::IpAddress& ip)
{
    _ip = ip;
}

void Ntw::ClientInfo::setPort(unsigned short port)
{
    _port = port;
}

std::string Ntw::ClientInfo::toString() const
{
    std::ostringstream oss;
    oss << _ip.toString() << ":" << _port;
    return oss.str();
}

void Ntw::NetworkManager::addClient(const sf::IpAddress& ip, unsigned short port)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    _clients.push_back(std::make_unique<ClientInfo>(ip, port));
    std::cout << "[NetworkManager] Nouveau client : " << ip << ":" << port << std::endl;
}

void Ntw::NetworkManager::removeClient(const sf::IpAddress& ip, unsigned short port)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    _clients.erase(
        std::remove_if(_clients.begin(), _clients.end(),
            [&](const std::unique_ptr<ClientInfo>& client) {
                return client->getIp() == ip && client->getPort() == port;
            }),
        _clients.end()
    );
    std::cout << "[NetworkManager] Client supprimé : " << ip << ":" << port << std::endl;
}

const std::vector<std::unique_ptr<Ntw::ClientInfo>>& Ntw::NetworkManager::getClients() const
{
    return _clients;
}

std::mutex& Ntw::NetworkManager::getClientsMutex()
{
    return _clientsMutex;
}

