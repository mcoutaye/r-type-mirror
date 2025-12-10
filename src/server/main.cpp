/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** main
*/

#include "server.hpp"

int main()
{
    Server server(53000);
    const auto tickRate = std::chrono::milliseconds(16); // ~60 Hz
    auto lastTime = std::chrono::steady_clock::now();

    while (server._running) {
        auto now = std::chrono::steady_clock::now();
        if (now - lastTime >= tickRate) {
            server._timer.updateClock();
            server.processInputs();
            server.update();
            server.broadcast();
            lastTime = now;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

// int main()
// {
//     UdpServer server(53000);
//     if (!server.start())
//         return -1;
//     uint32_t tick = 0;
//     auto last = std::chrono::steady_clock::now();
//     while (true) {
//         std::pair<int, InputState> input;
//         while (server.receivedInputs.pop(input)) {
//             std::cout << "J" << input.first << " → "
//           << "↑" << (int)input.second.up
//           << " ↓" << (int)input.second.down
//           << " ←" << (int)input.second.left
//           << " →" << (int)input.second.right
//           << "  Shoot:" << (int)input.second.shoot
//           << '\n';
//         }
//         if (std::chrono::steady_clock::now() - last >= 16ms) {
//             std::vector<EntityUpdate> updates = {
//                 {1, 100.0f, 200.0f},
//                 {2, 300.0f, 400.0f},
//                 {5, 150.0f, 150.0f}
//             };
//             std::vector<uint8_t> buffer(sizeof(EntityUpdate) * updates.size());
//             std::memcpy(buffer.data(), updates.data(), buffer.size());
//             for (const auto& client : server.getClients()) {
//                 if (client)
//                     server.packetsToSend.push({
//                         client->address,
//                         client->port,
//                         buffer
//                     });
//             }
//         }
//         std::this_thread::sleep_for(1ms);
//     }
// }
