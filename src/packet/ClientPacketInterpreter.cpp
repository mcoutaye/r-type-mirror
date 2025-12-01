/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** ClientPacketInterpreter
*/

#include "../../include/packet/ClientPacketInterpreter.hpp"
#include <iostream>
#include <sstream>

Ntw::ClientPacketInterpreter::ClientPacketInterpreter(
    UdpReceiver& receiver, GuiLoop& gameLoop)
: PacketInterpreter(receiver), _gameLoop(gameLoop)
{
}

void Ntw::ClientPacketInterpreter::onInputPacket(const Protocol::InputPacket& input)
{
    (void)input;
    std::cerr << "[Client] Unexpected INPUT packet\n";
}

void Ntw::ClientPacketInterpreter::onPositionPacket(const Protocol::PositionPacket& pos)
{
    std::cout << "[Client] Position update: EntityID=" << +pos.getEntityId()
                << ", X=" << pos.getX() << ", Y=" << pos.getY() << std::endl;
}

void Ntw::ClientPacketInterpreter::onJoinPacket(const ReceivedPacket packet)
{
    (void)packet;
    std::cout << "[Client] Joined the server\n";
}

void Ntw::ClientPacketInterpreter::onLeavePacket(const ReceivedPacket packet)
{
    (void)packet;
    std::cout << "[Client] Server closed the connection\n";
}
