/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** DisplayLoop
*/

#include "DisplayLoop.hpp"
#include <iostream>
#include "../../include/protocol/Protocol.hpp"
#include "../../include/network/UdpSender.hpp"


DisplayLoop::DisplayLoop(Ntw::UdpSender &Sender, int port, sf::IpAddress ip)
: _sender(Sender), _port(port), _ip(ip)
{
    std::cout << "[DisplayLoop] started " << std::endl;
}


DisplayLoop::~DisplayLoop()
{
    stop();
}

void DisplayLoop::start()
{
    if (!_running) {
        _running = true;
        _thread = std::thread(&DisplayLoop::displayLoop, this);
    }
}

void DisplayLoop::stop()
{
    _running = false;
    if (_thread.joinable())
        _thread.join();
}

void DisplayLoop::join()
{
    if (_thread.joinable())
        _thread.join();
}

void DisplayLoop::displayLoop()
{
    std::cout << "[DisplayLoop] Started - 60 FPS target\n";
    while (_running) {
        float dt = _clock.restart().asSeconds();
        (void)dt;
        // TODO: Ajouter ici la logique de rendu/affichage du jeu
        Protocol::InputPacket input(1, 0x01);
        std::vector<char> server_packet = Protocol::Protocol::createInputPacket(input);
        _sender.sendTo(server_packet, _ip, _port);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    std::cout << "[DisplayLoop] Stopped\n";
}
