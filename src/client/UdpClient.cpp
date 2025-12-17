/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** UdpClient
*/

#include "network/UdpClient.hpp"

Nwk::UdpClient::UdpClient(sf::IpAddress serverIp, unsigned short serverPort)
: m_serverIp(serverIp), m_serverPort(serverPort) {}

Nwk::UdpClient::~UdpClient()
{
    stop();
    join();
    if (m_socket.getLocalPort() != 0) {
        m_socket.unbind();
        std::cout << "[UDP Client] Port local libéré (destructeur)\n";
    }
}

bool Nwk::UdpClient::start(unsigned short localPort)
{
    if (m_socket.bind(localPort) != sf::Socket::Done) {
        std::cerr << "[UDP Client] Impossible de bind le port local\n";
        return false;
    }
    m_socket.setBlocking(false);
    std::cout << "[UDP Client] Connecté à " << m_serverIp << ":" << m_serverPort
              << " (local port: " << m_socket.getLocalPort() << ")\n";
    m_running = true;
    m_sendThread = std::thread(&Nwk::UdpClient::sendThread, this);
    m_recvThread = std::thread(&Nwk::UdpClient::receiveThread, this);
    return true;
}

void Nwk::UdpClient::stop()
{
    m_running = false;
    m_socket.unbind();
}

void Nwk::UdpClient::join()
{
    if (m_sendThread.joinable())
        m_sendThread.join();
    if (m_recvThread.joinable())
        m_recvThread.join();
}

void Nwk::UdpClient::sendThread()
{
    InputState input;
    while (m_running) {
        if (inputsToSend.pop(input))
            m_socket.send(&input, sizeof(input), m_serverIp, m_serverPort);
        else
            std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
}

void Nwk::UdpClient::receiveThread()
{
    std::vector<uint8_t> buffer(2048);
    sf::IpAddress sender;
    unsigned short port;
    while (m_running) {
        std::size_t received = 0;
        auto status = m_socket.receive(buffer.data(), buffer.size(), received, sender, port);
        if (status == sf::Socket::Done && sender == m_serverIp && port == m_serverPort) {
            if (received > 0 && received % sizeof(EntityUpdate) == 0) {
                std::vector<EntityUpdate> updates;
                updates.resize(received / sizeof(EntityUpdate));
                std::memcpy(updates.data(), buffer.data(), received);
                receivedUpdates.push(std::move(updates));
            }
        } else if (status == sf::Socket::NotReady)
            std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
}
