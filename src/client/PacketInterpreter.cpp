/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** PacketInterpreter
*/

#include "PacketInterpreter.hpp"
#include <iostream>

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
            if (packet._data.empty()) continue;
            uint8_t msgType = static_cast<uint8_t>(packet._data[0]);
            std::cout << "[Interpreter] Message type " << +msgType
                      << " from " << packet._sender << ":" << packet._port << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}
