/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** main
*/

#include "network/UdpClient.hpp"
#include "network/TcpClient.hpp"
#include "network/NetworkProtocol.hpp"
#include <iostream>
#include <random>

int main()
{
    Nwk::TcpClient tcp("127.0.0.1", 53001);
    Nwk::UdpClient udp("127.0.0.1", 53000);
    if (!tcp.start() || !udp.start())
        return 1;
    std::cout << "Client TCP + UDP lancé. Simulation d'envoi d'inputs aléatoires via UDP.\n";
    std::thread simThread([&]() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(0, 1);
        while (tcp.isConnected()) {
            Nwk::InputState in;
            in.up = dis(gen);
            in.down = dis(gen);
            in.left = dis(gen);
            in.right = dis(gen);
            in.shoot = dis(gen);
            udp.inputsToSend.push(in);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    });
    std::thread recvThread([&]() {
        while (tcp.isConnected()) {
            std::vector<Nwk::EntityUpdate> updates;
            if (udp.receivedUpdates.pop(updates)) {
                std::cout << "Reçu snapshot du serveur :\n";
                for (const auto& u : updates)
                    std::cout << "Entity " << u.entityId << " : (" << u.x << ", " << u.y << ")\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
    while (tcp.isConnected())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "[Client] TCP déconnecté → arrêt UDP\n";
    simThread.join();
    recvThread.join();
    std::cout << "Client arrêté.\n";
    return 0;
}
