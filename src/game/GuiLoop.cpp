/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** GuiLoop
*/

#include "../../include/game/GuiLoop.hpp"
#include "../../include/protocol/Protocol.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include <sstream>

GuiLoop::GuiLoop(Ntw::UdpSender& sender)
: GameLoop(sender)
{
}

void GuiLoop::gameLoop()
{
    std::cout << "[GuiLoop] Started - 60 FPS target\n";
    // uint8_t playerId = 1;
    while (_running) {
        // uint32_t keys = 0;
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) keys |= 0x01;
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) keys |= 0x02;
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) keys |= 0x04;
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) keys |= 0x08;
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) keys |= 0x10;
        // Protocol::InputPacket input(playerId, keys);
        // std::vector<char> packet = Protocol::Protocol::createInputPacket(input);
        // _sender.sendTo(packet, sf::IpAddress("127.0.0.1"), 55002);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    std::cout << "[GuiLoop] Stopped\n";
}
