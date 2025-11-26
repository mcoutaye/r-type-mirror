/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** main
*/

#include "../../include/args/Args.hpp"
#include "NetworkManager.hpp"
#include <iostream>
#include <SFML/System.hpp>

int main(int argc, char *argv[])
{
    Args args(argc, argv, Args::Mode::SERVER);
    auto config = args.parse();
    Ntw::NetworkManager network(config.port);
    network.start();
    network.join();
    return 0;
}
