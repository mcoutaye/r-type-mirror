/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** main
*/

#include "../../include/args/Args.hpp"
#include "NetworkManager.hpp"
#include "GameLoop.hpp"
#include <iostream>
#include <SFML/System.hpp>

int main(int argc, char *argv[])
{
    Args args(argc, argv, Args::Mode::SERVER);
    Args::Config config = args.parse();
    Ntw::NetworkManager network(config.port);
    GameLoop game(network.getSender(), config.port, config.machine);
    network.start();
    game.start();
    network.join();
    game.join();
    return 0;
}
