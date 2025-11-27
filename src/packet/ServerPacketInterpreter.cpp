/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** ServerPacketInterpreter
*/

#include "../../include/packet/ServerPacketInterpreter.hpp"
#include <iostream>


Ntw::ServerPacketInterpreter::ServerPacketInterpreter(
    UdpReceiver& receiver, Ntw::NetworkManager& networkManager, GamePlayLoop& gameLoop)
: PacketInterpreter(receiver), _networkManager(networkManager), _gameLoop(gameLoop)
{
}

void Ntw::ServerPacketInterpreter::onInputPacket(const Protocol::InputPacket& input)
{
    std::cout << "[Server] Player " << +input.getPlayerId()
                << " sent input: 0x" << std::hex << input.getKeys() << std::dec << std::endl;
}

void Ntw::ServerPacketInterpreter::onPositionPacket(const Protocol::PositionPacket& pos)
{
    (void)pos;
    std::cerr << "[Server] Unexpected POSITION packet\n";
}

void Ntw::ServerPacketInterpreter::onJoinPacket(const ReceivedPacket packet)
{
    std::cout << "[Server] New client joined: "
                << packet._sender.toString() << ":" << packet._port << std::endl;
    _networkManager.addClient(packet._sender, packet._port);
}

void Ntw::ServerPacketInterpreter::onLeavePacket(const ReceivedPacket packet)
{
    std::cout << "[Server] Client left: "
                << packet._sender.toString() << ":" << packet._port << std::endl;
    _networkManager.removeClient(packet._sender, packet._port);
}
