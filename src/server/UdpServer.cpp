/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** UdpServer
*/

#include "UdpServer.hpp"
#include <algorithm>

UdpServer::UdpServer(unsigned short port) : m_port(port) {}

UdpServer::~UdpServer()
{
    stop();
    join();
}

bool UdpServer::start()
{
    if (m_socket.bind(m_port) != sf::Socket::Done) {
        std::cerr << "Impossible de binder le port UDP " << m_port << std::endl;
        return false;
    }
    m_socket.setBlocking(false);
    std::cout << "Serveur UDP SFML demarre sur le port " << m_port << std::endl;
    m_running = true;
    m_recvThread = std::thread(&UdpServer::receiveThread, this);
    m_sendThread = std::thread(&UdpServer::sendThread, this);
    return true;
}

void UdpServer::stop()
{
    m_running = false;
    m_socket.unbind();
}

void UdpServer::join()
{
    if (m_recvThread.joinable())
        m_recvThread.join();
    if (m_sendThread.joinable())
        m_sendThread.join();
}

ClientInfo* UdpServer::findClient(const sf::IpAddress& ip, unsigned short port)
{
    for (auto& client : m_clients)
        if (client && client->address == ip && client->port == port)
            return client.get();
    return nullptr;
}

bool UdpServer::addClient(sf::IpAddress ip, unsigned short port, int playerId)
{
    if (m_clientCount >= MAX_CLIENTS) {
        std::cerr << "Serveur UDP plein !\n";
        return false;
    }
    if (findClient(ip, port))
        return true;
    if (playerId == -1)
        playerId = static_cast<int>(m_clients.size());
    m_clients.push_back(std::make_unique<ClientInfo>(ip, port, playerId));
    m_clientCount++;
    std::cout << "Joueur " << playerId << " ajouté en UDP (" << ip << ":" << port << ")\n";
    return true;
}

void UdpServer::removeClient(int playerId)
{
    auto it = std::remove_if(m_clients.begin(), m_clients.end(),
        [playerId](const auto& c) { return c && c->playerId == playerId; });
    if (it != m_clients.end()) {
        m_clients.erase(it, m_clients.end());
        m_clientCount--;
        std::cout << "Joueur " << playerId << " retiré du serveur UDP\n";
    }
}

void UdpServer::receiveThread()
{
    sf::Packet packet;
    sf::IpAddress senderIp;
    unsigned short senderPort;
    while (m_running) {
        auto status = m_socket.receive(packet, senderIp, senderPort);
        if (status == sf::Socket::Done) {
            ClientInfo* client = findClient(senderIp, senderPort);
            if (client && packet.getDataSize() == sizeof(InputState)) {
                InputState input;
                std::memcpy(&input, packet.getData(), sizeof(InputState));
                receivedInputs.push({client->playerId, input});
            } else
                addClient(senderIp, senderPort, static_cast<int>(m_clients.size()));
            packet.clear();
        } else if (status == sf::Socket::NotReady)
            std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
}

void UdpServer::sendThread()
{
    PacketToSend toSend;
    while (m_running) {
        if (packetsToSend.pop(toSend))
            m_socket.send(toSend.data.data(), toSend.data.size(), toSend.destIp, toSend.destPort);
        else
            std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
}
