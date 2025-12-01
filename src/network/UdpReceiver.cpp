/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** UdpReceiver
*/

#include "../../include/network/UdpReceiver.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

Ntw::ReceivedPacket::ReceivedPacket(sf::IpAddress sender, unsigned short port, std::vector<char> data)
: _sender(sender), _port(port), _data(data)
{
}

Ntw::ReceivedPacket::ReceivedPacket()
: _port(0)
{
}

Ntw::UdpReceiver::UdpReceiver(unsigned short port)
: _port(port)
{
    if (_port != 0 && _socket.bind(_port) != sf::Socket::Done) {
        std::cerr << "Erreur bind port " << _port << std::endl;
    }
    _socket.setBlocking(false);
}

Ntw::UdpReceiver::~UdpReceiver()
{
    stop();
}

void Ntw::UdpReceiver::start()
{
    if (!_running) {
        _running = true;
        _thread = std::thread(&UdpReceiver::receiveLoop, this);
    }
}

void Ntw::UdpReceiver::stop()
{
    _running = false;
    if (_thread.joinable())
        _thread.join();
}

void Ntw::UdpReceiver::join()
{
    if (_thread.joinable())
        _thread.join();
}

void Ntw::UdpReceiver::printPacketDebug(const sf::IpAddress& sender, unsigned short port,
                                   const std::vector<char>& data) const
{
    if (!_debug)
        return;
    std::ostringstream oss;
    oss << "[UDP RECV] ← " << sender << ":" << port
        << " | Taille: " << std::setw(3) << data.size() << " octets";
    if (!data.empty()) {
        oss << " | Type: 0x" << std::hex << std::uppercase
            << std::setw(2) << std::setfill('0') << (static_cast<int>(data[0]) & 0xFF);
        if (data.size() > 1) {
            oss << " | Data: ";
            for (size_t i = 0; i < std::min(data.size(), size_t(16)); ++i) {
                oss << std::hex << std::setw(2) << std::setfill('0')
                    << (static_cast<int>(data[i]) & 0xFF) << " ";
            }
            if (data.size() > 16)
                oss << "...";
        }
    }
    oss << std::dec << std::setfill(' ');
    std::cout << oss.str() << std::endl;
}

void Ntw::UdpReceiver::receiveLoop()
{
    char buffer[2048];
    std::size_t received;
    sf::IpAddress sender;
    unsigned short senderPort;
    while (_running) {
        sf::Socket::Status status = _socket.receive(buffer, sizeof(buffer), received, sender, senderPort);
        if (status == sf::Socket::Done && received > 0) {
            std::vector<char> data(buffer, buffer + received);
            printPacketDebug(sender, senderPort, data);
            auto packet = std::make_unique<ReceivedPacket>(
                sender,
                senderPort,
                std::vector<char>(buffer, buffer + received)
            );
            std::lock_guard<std::mutex> lock(_mutex);
            _packetQueue.push(std::move(packet));
        }
        if (status == sf::Socket::NotReady)
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        if (status != sf::Socket::Done)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

bool Ntw::UdpReceiver::getPacket(ReceivedPacket& packet)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_packetQueue.empty())
        return false;
    packet._sender = _packetQueue.front()->_sender;
    packet._port = _packetQueue.front()->_port;
    packet._data = std::move(_packetQueue.front()->_data);
    _packetQueue.pop();
    return true;
}

unsigned short Ntw::UdpReceiver::getPort() const
{
    return _port;
}

void Ntw::UdpReceiver::setDebug(bool enabled)
{
    _debug = enabled;
}

sf::UdpSocket& Ntw::UdpReceiver::getSocket()
{
    return _socket;
}
