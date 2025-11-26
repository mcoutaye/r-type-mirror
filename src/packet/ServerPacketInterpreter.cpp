/*
** EPITECH PROJECT, 2025
** R-Type - ServerPacketInterpreter
*/
#include "../../include/packet/ServerPacketInterpreter.hpp"
#include <iostream>

Ntw::ServerPacketInterpreter::ServerPacketInterpreter(UdpReceiver& receiver)
: PacketInterpreter(receiver)
{
}

void Ntw::ServerPacketInterpreter::onInputPacket(const Protocol::InputPacket& input)
{
    std::cout << "[Server] Player " << +input.getPlayerId()
                << " sent input: " << input.getKeys() << "\n";
}

void Ntw::ServerPacketInterpreter::onPositionPacket(const Protocol::PositionPacket& pos)
{
    (void)pos;
    std::cerr << "[Server] Unexpected POSITION packet\n";
}
