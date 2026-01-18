/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** main
*/

#include "client.hpp"
#include "texturePacker.hpp"


int main(int ac, char **av)
{
    sf::IpAddress ip = "127.0.0.1";
    if (ac == 2)
        ip = sf::IpAddress(av[1]);

    Client client(ip);

    client._resourceManager.initialize();

    while (client._running) {
        client._timer.updateClock();
        client.processInput();
        client.update();
        client.render();
    }
}
