/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** GameLoop
*/

#include "GameLoop.hpp"
#include <iostream>
#include "../../include/protocol/Protocol.hpp"
#include "../../include/network/UdpSender.hpp"

GameLoop::GameLoop(Ntw::UdpSender &Sender, int port, sf::IpAddress ip)
: _sender(Sender), _port(port), _ip(ip)
{
    std::cout << "[GameLoop] GameLoop started " <<std::endl;
}


GameLoop::~GameLoop()
{
    stop();
}



void GameLoop::start()
{
    if (!_running) {
        _running = true;
        _thread = std::thread(&GameLoop::gameLoop, this);
    }
}

void GameLoop::stop()
{
    _running = false;
    if (_thread.joinable())
        _thread.join();
}

void GameLoop::join()
{
    if (_thread.joinable())
        _thread.join();
}

void GameLoop::gameLoop()
{
    std::cout << "[GameLoop] Started - 60 FPS target\n";
    while (_running) {
        float dt = _clock.restart().asSeconds();
        (void)dt;
        Protocol::PositionPacket position(1, 0.3, 0.4, 0.5);
        std::vector<char> client_packet = Protocol::Protocol::createPositionPacket(position);
        _sender.sendTo(client_packet, _ip, _port);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    std::cout << "[GameLoop] Stopped\n";
}
