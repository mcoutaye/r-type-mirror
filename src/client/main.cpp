/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** main
*/

#include "UdpClient.hpp"
#include <chrono>
using namespace std::chrono_literals;

int main()
{
    UdpClient client("127.0.0.1", 53000);
    if (!client.start())
        return -1;
    while (true) {
        InputState input{1, 0, 0, 0, 0};
        client.inputsToSend.push(input);
        std::vector<EntityUpdate> updates;
        while (client.receivedUpdates.pop(updates)) {
            std::cout << "Reçu " << updates.size() << " entités :\n";
            for (const auto& e : updates)
                std::cout << "  Entity " << e.entityId << " → (" << e.x << ", " << e.y << ")\n";
        }
        std::this_thread::sleep_for(100ms);
    }
    client.stop();
    client.join();
    return 0;
}