/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** UdpClient
*/

#include "UdpClient.hpp"

UdpClient::UdpClient(sf::IpAddress serverIp, unsigned short serverPort)
: m_serverIp(serverIp), m_serverPort(serverPort) {}

UdpClient::~UdpClient()
{
    stop();
    join();
}

bool UdpClient::start(unsigned short localPort)
{
    if (m_socket.bind(localPort) != sf::Socket::Done) {
        std::cerr << "[UDP Client] Impossible de bind le port local\n";
        return false;
    }
    m_socket.setBlocking(false);
    std::cout << "[UDP Client] Connecté à " << m_serverIp << ":" << m_serverPort
              << " (local port: " << m_socket.getLocalPort() << ")\n";
    m_running = true;
    m_sendThread = std::thread(&UdpClient::sendThread, this);
    m_recvThread = std::thread(&UdpClient::receiveThread, this);
    return true;
}

void UdpClient::stop()
{
    m_running = false;
    m_socket.unbind();
}

void UdpClient::join()
{
    if (m_sendThread.joinable())
        m_sendThread.join();
    if (m_recvThread.joinable())
        m_recvThread.join();
}

void UdpClient::sendThread()
{
    InputState input;
    sf::Clock pingClock;
    while (true) {
        bool popped = inputsToSend.tryPop(input);
        if (popped) {
            if (input.tick != MAGIC_TICK_CLIENT_QUIT) {
                _timer.updateClock();
                input.tick = _timer.getCurrentFrame();
            }
            m_socket.send(&input, sizeof(input), m_serverIp, m_serverPort);
        }

        if (pingClock.getElapsedTime().asMilliseconds() >= 1000) {
            sendPing();
            pingClock.restart();
        }

        if (!m_running && !popped)
            break;

        if (!popped)
            std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
}

void UdpClient::receiveThread()
{
    std::vector<uint8_t> buffer(2048);
    sf::IpAddress sender;
    unsigned short port;
    while (m_running) {
        std::size_t received = 0;
        auto status = m_socket.receive(buffer.data(), buffer.size(), received, sender, port);
        if (status == sf::Socket::Done && sender == m_serverIp && port == m_serverPort) {
            if (received == sizeof(PongPacket)) {
                PongPacket pong;
                std::memcpy(&pong, buffer.data(), sizeof(PongPacket));
                if (pong.type == static_cast<uint8_t>(ControlPacketType::Pong)) {
                    auto now = std::chrono::steady_clock::now();
                    bool found = false;
                    std::chrono::steady_clock::time_point sentAt;
                    {
                        std::lock_guard<std::mutex> lock(_pingMutex);
                        auto it = _pendingPings.find(pong.pingId);
                        if (it != _pendingPings.end()) {
                            sentAt = it->second;
                            _pendingPings.erase(it);
                            found = true;
                        }
                    }
                    if (found) {
                        auto rtt = std::chrono::duration_cast<std::chrono::milliseconds>(now - sentAt).count();
                        _lastPingMs.store(static_cast<int>(rtt));
                    }
                    continue;
                }
            }
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


void UdpClient::sendPing()
{
    pruneOldPings();

    PingPacket ping;
    ping.pingId = _nextPingId.fetch_add(1);

    {
        std::lock_guard<std::mutex> lock(_pingMutex);
        _pendingPings[ping.pingId] = std::chrono::steady_clock::now();
    }

    m_socket.send(&ping, sizeof(ping), m_serverIp, m_serverPort);
}

void UdpClient::pruneOldPings()
{
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(_pingMutex);
    for (auto it = _pendingPings.begin(); it != _pendingPings.end();) {
        if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count() > 5) {
            it = _pendingPings.erase(it);
        } else {
            ++it;
        }
    }
}
