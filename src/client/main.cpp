/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** main
*/

#include "Args.hpp"
#include "ClientManager.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    Args args(argc, argv);
    auto config = args.parse();
    Ntw::ClientManager client(config.machine, config.port);
    client.start();
    auto& sender = client.getSender();
    std::vector<char> magic = {'R', 'T', 'Y', 'P'};
    sender.sendTo(magic, sf::IpAddress(config.machine), config.port);
    client.join();
    return 0;
}
