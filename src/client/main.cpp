/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** main
*/

#include "../../include/args/Args.hpp"
#include "ClientManager.hpp"
#include "DisplayLoop.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    Args args(argc, argv, Args::Mode::CLIENT);
    Args::Config config = args.parse();
    Ntw::ClientManager client(config.machine, config.port);
    DisplayLoop gui(client.getSender(), config.port, config.machine);
    client.start();
    gui.start();
    // auto& sender = client.getSender();
    client.join();
    gui.join();
    return 0;
}
