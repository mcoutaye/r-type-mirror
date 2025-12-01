/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** PacketInterpreter
*/

#include "../../include/packet/PacketInterpreter.hpp"
#include "../../include/protocol/Protocol.hpp"
#include <iostream>
#include <sstream>

Ntw::PacketInterpreter::PacketInterpreter(UdpReceiver& receiver)
: _receiver(receiver)
{
}

Ntw::PacketInterpreter::~PacketInterpreter()
{
    stop();
}

void Ntw::PacketInterpreter::start()
{
    if (!_running) {
        _running = true;
        _thread = std::thread(&PacketInterpreter::interpreterLoop, this);
    }
}

void Ntw::PacketInterpreter::stop()
{
    _running = false;
    if (_thread.joinable())
        _thread.join();
}

void Ntw::PacketInterpreter::join()
{
    if (_thread.joinable())
        _thread.join();
}

void Ntw::PacketInterpreter::setOnPositionUpdate(OnPositionUpdate callback)
{
    _onPositionUpdate = callback;
}

void Ntw::PacketInterpreter::interpreterLoop()
{
    ReceivedPacket packet;
    while (_running) {
        while (_receiver.getPacket(packet)) {
            std::lock_guard<std::mutex> lock(_mutex);
            _packetBuffer.push(packet);
        }
        while (!_packetBuffer.empty()) {
            std::lock_guard<std::mutex> lock(_mutex);
            packet = std::move(_packetBuffer.front());
            _packetBuffer.pop();
            if (packet._data.empty())
                continue;
            if (!Protocol::Protocol::isValidPacket(packet._data)) {
                std::cerr << "[PacketInterpreter] Invalid packet (no magic)\n";
                continue;
            }
            Protocol::MessageType msgType = Protocol::Protocol::getMessageType(packet._data);
            std::vector<char> data = Protocol::Protocol::getPacketData(packet._data);
            switch (msgType) {
                case Protocol::MessageType::INPUT: {
                    const Protocol::InputPacket& input = Protocol::InputPacket::deserialize(data);
                    onInputPacket(input);
                    break;
                }
                case Protocol::MessageType::POSITION: {
                    const Protocol::PositionPacket& pos = Protocol::PositionPacket::deserialize(data);
                    onPositionPacket(pos);
                    if (_onPositionUpdate)
                        _onPositionUpdate(pos.getEntityId(), pos.getX(), pos.getY());
                    break;
                }
                case Protocol::MessageType::JOIN:
                    onJoinPacket(packet);
                    break;
                case Protocol::MessageType::LEAVE:
                    onLeavePacket(packet);
                    break;
                default:
                    std::cerr << "[PacketInterpreter] Unknown message type: "
                                << +static_cast<uint8_t>(msgType) << "\n";
            }
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}
