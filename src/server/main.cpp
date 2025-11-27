/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** main
*/

#include "../../include/args/Args.hpp"
#include "../../include/network/NetworkManager.hpp"
#include "../../include/network/UdpReceiver.hpp"
#include "../../include/network/UdpSender.hpp"
#include "../../include/game/GamePlayLoop.hpp"
#include "../../include/packet/ServerPacketInterpreter.hpp"

int main(int argc, char* argv[])
{
    Args args(argc, argv, Args::Mode::SERVER);
    Args::Config config = args.parse();
    Ntw::NetworkManager networkManager;
    Ntw::UdpReceiver receiver(config.port);
    Ntw::UdpSender sender(receiver.getSocket());
    GamePlayLoop gameLoop(sender, networkManager);
    Ntw::ServerPacketInterpreter interpreter(receiver, networkManager, gameLoop);
    receiver.start();
    interpreter.start();
    gameLoop.start();
    gameLoop.join();
    interpreter.join();
    receiver.join();
    return 0;
}
