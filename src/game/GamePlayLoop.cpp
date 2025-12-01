/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** GamePlayLoop
*/

#include "../../include/game/GamePlayLoop.hpp"
#include "../../include/protocol/Protocol.hpp"
#include <iostream>
#include <sstream>

GamePlayLoop::GamePlayLoop(Ntw::UdpSender& sender, Ntw::NetworkManager& networkManager)
: GameLoop(sender), _networkManager(networkManager)
{
}

void GamePlayLoop::gameLoop()
{
    std::cout << "[GamePlayLoop] Started - 60 FPS target\n";
    float x = 100.0f;
    float y = 200.0f;
    uint8_t entityId = 1;
    while (_running) {
        x += 1.0f;
        Protocol::PositionPacket pos(entityId, x, y);
        std::vector<char> packet = Protocol::Protocol::createPositionPacket(pos);
        std::lock_guard<std::mutex> lock(_networkManager.getClientsMutex());
        for (const auto& client : _networkManager.getClients())
            _sender.sendTo(packet, client->getIp(), client->getPort());
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    std::cout << "[GamePlayLoop] Stopped\n";
}
