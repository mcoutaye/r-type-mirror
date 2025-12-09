/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** main
*/

#include "GameLoop.hpp"
#include "UdpServer.hpp"
#include <iostream>

int main()
{
    UdpServer udp(53000);
    if (!udp.start())
        return 1;
    GameLoop game(udp.receivedInputs, udp.packetsToSend);
    game.start();
    game.join();
    udp.join();
}
