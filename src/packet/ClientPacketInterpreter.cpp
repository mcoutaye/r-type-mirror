/*
** EPITECH PROJECT, 2025
** R-Type - ClientPacketInterpreter
*/
#include "../../include/packet/ClientPacketInterpreter.hpp"
#include <iostream>

Ntw::ClientPacketInterpreter::ClientPacketInterpreter(UdpReceiver& receiver)
: PacketInterpreter(receiver)
{
}

void Ntw::ClientPacketInterpreter::onInputPacket(const Protocol::InputPacket& input)
{
    (void)input;
    std::cerr << "[Client] Unexpected INPUT packet\n";
}

void Ntw::ClientPacketInterpreter::onPositionPacket(const Protocol::PositionPacket& pos)
{
    std::cout << "[Client] Entity " << +pos.getEntityId()
                << " is at (" << pos.getX() << ", " << pos.getY() << ")\n";
}
