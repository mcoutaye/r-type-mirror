/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** main
*/

#include "tcp.hpp"

int main()
{
    const int port = 8000;
    Server server(port);

    if (!server.init()) {
        std::cerr << "Failed to initialize server." << std::endl;
        return 84;
    }

    return server.run();
}
