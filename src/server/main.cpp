/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** main
*/

#include "UdpServer.hpp"
#include "TcpServer.hpp"
#include "NetworkProtocol.hpp"
#include <iostream>
#include <csignal>
#include <random>
#include <mutex>

volatile bool g_running = true;
void signalHandler(int) { g_running = false; }

int main()
{
    std::signal(SIGINT, signalHandler);
    TcpServer tcp(53001);
    UdpServer udp(53000);
    if (!tcp.start() || !udp.start())
        return 1;
    std::cout << "Serveur TCP + UDP lancé. Simulation d'envoi de positions aléatoires via UDP.\n";
    std::thread simThread([&]() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1280.0f);
        while (g_running) {
            std::vector<EntityUpdate> snapshot;
            for (uint32_t id = 1; id <= 5; ++id)
                snapshot.push_back({id, dis(gen), dis(gen)});
            auto &clients = udp.getClients();
            for (const auto& client : clients) {
                PacketToSend p;
                p.destIp = client->address;
                p.destPort = client->port;
                p.data.resize(snapshot.size() * sizeof(EntityUpdate));
                std::memcpy(p.data.data(), snapshot.data(), p.data.size());
                udp.packetsToSend.push(std::move(p));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });
    while (g_running) {
        std::pair<int, InputState> input;
        while (udp.receivedInputs.pop(input)) {
            std::cout << "[Server] Input player " << input.first
                    << " → ↑" << int(input.second.up)
                    << " ↓" << int(input.second.down)
                    << " ←" << int(input.second.left)
                    << " →" << int(input.second.right)
                    << " Shoot:" << int(input.second.shoot) << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    simThread.join();
    std::cout << "Serveur arrêté.\n";
    return 0;
}
