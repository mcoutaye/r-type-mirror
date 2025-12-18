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
    std::lock_guard<std::mutex> lock(m_clientsMutex);
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
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    auto it = std::remove_if(m_clients.begin(), m_clients.end(),
        [playerId](const auto& c) { return c && c->playerId == playerId; });
    if (it != m_clients.end()) {
        m_clients.erase(it, m_clients.end());
        m_clientCount--;
        std::cout << "Joueur " << playerId << " retiré du serveur UDP\n";
    }
}

std::size_t UdpServer::getClientCount() const
{
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    return m_clientCount;
}

const ClientInfo UdpServer::getClient(std::size_t ID) const
{
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    if (ID >= m_clientCount)
        return ClientInfo(sf::IpAddress::None, 0, -1);
    return *m_clients[ID].get();
}

std::vector<ClientInfo> UdpServer::getClients() const
{
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    std::vector<ClientInfo> clients;
    clients.reserve(m_clients.size());
    for (const auto& client : m_clients) {
        if (client)
            clients.push_back(*client);
    }
    return clients;
}

PacketToSend UdpServer::createPacket(std::size_t clientID, std::vector<uint8_t> data)
{
    const ClientInfo info = getClient(clientID);
    PacketToSend packet {sf::IpAddress::None, 0};

    if (info.playerId < 0)
        return packet;
    packet.destIp = info.address;
    packet.destPort = info.port;
    packet.data = data;
    return packet;
}

std::queue<PacketToSend> UdpServer::createEveryonePacket(std::vector<uint8_t> data)
{
    std::queue<PacketToSend> queue;
    std::vector<ClientInfo> clients = getClients();

    for (const auto& client : clients) {
        PacketToSend packet {sf::IpAddress::None, 0};
        packet.destIp = client.address;
        packet.destPort = client.port;
        packet.data = data;
        queue.push(packet);
    }
    return queue;
}

void UdpServer::receiveThread()
{
    std::vector<uint8_t> buffer(1024);
    sf::IpAddress senderIp;
    unsigned short senderPort;
    std::size_t received = 0;

    while (m_running) {
        auto status = m_socket.receive(buffer.data(), buffer.size(), received, senderIp, senderPort);
        if (status == sf::Socket::Done) {
            std::lock_guard<std::mutex> lock(m_clientsMutex);
            ClientInfo* client = findClient(senderIp, senderPort);
            if (!client) {
                if (m_clientCount < MAX_CLIENTS) {
                    int playerId = static_cast<int>(m_clients.size());
                    m_clients.push_back(std::make_unique<ClientInfo>(senderIp, senderPort, playerId));
                    m_clientCount++;
                    std::cout << "Joueur " << playerId << " ajouté en UDP (" << senderIp << ":" << senderPort << ")\n";
                    client = m_clients.back().get();
                } else {
                    std::cerr << "Serveur plein, paquet ignoré de " << senderIp << ":" << senderPort << std::endl;
                }
            }
            if (client && received == sizeof(InputState)) {
                InputState input;
                std::memcpy(&input, buffer.data(), sizeof(InputState));
                // std::cout << "Reçu input du joueur " << client->playerId << " (tick: " << input.tick << ")\n";
                receivedInputs.push({client->playerId, input});
            }
        } else if (status == sf::Socket::NotReady)
            std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
}

void UdpServer::sendThread()
{
    PacketToSend toSend;
    while (m_running) {
        if (packetsToSend.tryPop(toSend))
            m_socket.send(toSend.data.data(), toSend.data.size(), toSend.destIp, toSend.destPort);
        else
            std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
}
