/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** main
*/

#include "../../include/args/Args.hpp"
#include "../../include/network/NetworkManager.hpp"
#include "../../include/network/UdpReceiver.hpp"
#include "../../include/network/UdpSender.hpp"
#include "../../include/game/GuiLoop.hpp"
#include "../../include/packet/ClientPacketInterpreter.hpp"

int main(int argc, char* argv[]) {
    Args args(argc, argv, Args::Mode::CLIENT);
    Args::Config config = args.parse();
    Ntw::UdpReceiver receiver(55003);
    Ntw::UdpSender sender(receiver.getSocket());
    GuiLoop gameLoop(sender);
    Ntw::ClientPacketInterpreter interpreter(receiver, gameLoop);
    std::vector<char> joinPacket = Protocol::Protocol::createJoinPacket();
    sender.sendTo(joinPacket, sf::IpAddress(config.machine), config.port);
    receiver.start();
    interpreter.start();
    gameLoop.start();
    gameLoop.join();
    interpreter.join();
    receiver.join();
    return 0;
}
