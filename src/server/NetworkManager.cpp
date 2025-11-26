/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** NetworkManager
*/

#include "NetworkManager.hpp"
#include <iostream>

Ntw::NetworkManager::NetworkManager(unsigned short listenPort)
    : _receiver(listenPort), _interpreter(_receiver)
{
    std::cout << "[NetworkManager] Server initialized on port " << listenPort << std::endl;
}

Ntw::NetworkManager::~NetworkManager()
{
    stop();
}

Ntw::UdpSender& Ntw::NetworkManager::getSender()
{
    return _sender;
}

void Ntw::NetworkManager::start()
{
    std::cout << "[NetworkManager] Starting server threads...\n";
    _receiver.start();
    _interpreter.start();
    std::cout << "[NetworkManager] Server is fully running!\n";
    printServerInfo();
}

void Ntw::NetworkManager::stop()
{
    std::cout << "[NetworkManager] Stopping server...\n";
    _interpreter.stop();
    _receiver.stop();
    std::cout << "[NetworkManager] Server stopped cleanly.\n";
}

void Ntw::NetworkManager::join()
{
    std::cout << "[NetworkManager] Attente de la fin des threads...\n";
    _interpreter.join();
    _receiver.join();
    std::cout << "[NetworkManager] Tous les threads sont terminés.\n";
}

void Ntw::NetworkManager::printServerInfo() const
{
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════╗\n";
    std::cout << "║           SERVEUR R-TYPE EN LIGNE            ║\n";
    std::cout << "╚══════════════════════════════════════════════╝\n";
    std::cout << "   Port : " << _receiver.getPort() << "\n\n";
    std::cout << "   Connecte-toi avec l'une de ces adresses :\n";
    auto ips = getLocalIpAddresses();
    if (ips.empty())
        std::cout << "   Impossible de détecter les adresses locales.\n";
    else
        for (const auto& ip : ips) {
            std::cout << "   • " << ip.toString() << ":" << _receiver.getPort();
            if (ip == sf::IpAddress::getLocalAddress())
                std::cout << "  ← IP principale détectée";
            std::cout << "\n";
        }
    std::cout << "\n   • 127.0.0.1:" << _receiver.getPort() << "  ← localhost (toujours bon)\n";
    std::cout << "────────────────────────────────────────────────\n\n";
}

std::vector<sf::IpAddress> Ntw::NetworkManager::getLocalIpAddresses() const
{
    std::vector<sf::IpAddress> ips;
    sf::IpAddress mainIp = sf::IpAddress::getLocalAddress();
    if (mainIp != sf::IpAddress::None && mainIp != sf::IpAddress::LocalHost)
        ips.push_back(mainIp);
    sf::IpAddress publicIp = sf::IpAddress::getPublicAddress(sf::seconds(2));
    if (publicIp != sf::IpAddress::None && publicIp != sf::IpAddress::LocalHost)
        ips.push_back(publicIp);
    ips.push_back(sf::IpAddress("127.0.0.1"));
    std::sort(ips.begin(), ips.end());
    ips.erase(std::unique(ips.begin(), ips.end()), ips.end());
    return ips;
}
