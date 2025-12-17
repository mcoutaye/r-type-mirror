/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** main
*/

#include "client.hpp"

int main()
{
    Client client("127.0.0.1");

    while (client._running) {
        client._timer.updateClock();
        client.processInput();
        client.update();
        client.render();
    }
}
