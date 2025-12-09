/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** main
*/

#include "GuiLoop.hpp"
#include "UdpClient.hpp"
#include <SFML/Graphics.hpp>

int main()
{
    UdpClient udp("127.0.0.1", 53000);
    if (!udp.start())
        return 1;
    GuiLoop game(udp.inputsToSend, udp.receivedUpdates);
    game.start();
    game.join();
    udp.join();
}