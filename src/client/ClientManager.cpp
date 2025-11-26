/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** ClientManager
*/

#include "ClientManager.hpp"
#include <iostream>

Ntw::ClientManager::ClientManager(const std::string& serverIp, unsigned short serverPort)
    : _serverIp(serverIp), _serverPort(serverPort), _receiver(_serverPort), _interpreter(_receiver)
{
    std::cout << "[ClientManager] Client initialized, connecting to "
              << _serverIp << ":" << _serverPort << std::endl;
}

Ntw::ClientManager::~ClientManager()
{
    stop();
}

Ntw::UdpSender& Ntw::ClientManager::getSender()
{
    return _sender;
}

void Ntw::ClientManager::start()
{
    std::cout << "[ClientManager] Starting client threads...\n";
    _receiver.start();
    _receiver.setDebug(true);
    _interpreter.start();
    std::cout << "[ClientManager] Client is fully running!\n";
    printClientInfo();
}

void Ntw::ClientManager::stop()
{
    std::cout << "[ClientManager] Stopping client...\n";
    _interpreter.stop();
    _receiver.stop();
    std::cout << "[ClientManager] Client stopped cleanly.\n";
}

void Ntw::ClientManager::join()
{
    std::cout << "[ClientManager] Waiting for threads to finish...\n";
    _interpreter.join();
    _receiver.join();
    std::cout << "[ClientManager] All threads have terminated.\n";
}

void Ntw::ClientManager::printClientInfo() const
{
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════╗\n";
    std::cout << "║           CLIENT R-TYPE EN LIGNE             ║\n";
    std::cout << "╚══════════════════════════════════════════════╝\n";
    std::cout << "   Serveur : " << _serverIp << ":" << _serverPort << "\n\n";
    std::cout << "────────────────────────────────────────────────\n\n";
}
