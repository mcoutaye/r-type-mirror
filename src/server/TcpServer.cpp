/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** TcpServer
*/

#include "network/TcpServer.hpp"

Nwk::TcpServer::TcpServer(unsigned short port) : _port(port) {}

Nwk::TcpServer::~TcpServer()
{
    stop();
    join();
    if (_listener.getLocalPort() != 0)
        _listener.close();
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& client : _clients)
        if (client && client->getLocalPort() != 0)
            client->disconnect();
    _clients.clear();
    std::cout << "[TCP Server] Port " << _port << " libéré (destructeur)\n";
}

bool Nwk::TcpServer::start()
{
    if (_running)
        return false;
    if (_listener.listen(_port) != sf::Socket::Done) {
        std::cerr << "[TCP Server] Impossible d'écouter le port " << _port << std::endl;
        return false;
    }
    _listener.setBlocking(false);
    _selector.add(_listener);
    std::cout << "[TCP Server] Démarré sur le port " << _port << " (mode selector)\n";
    _running = true;
    _thread = std::thread(&Nwk::TcpServer::run, this);
    return true;
}

void Nwk::TcpServer::run()
{
    while (_running) {
        if (_selector.wait(sf::milliseconds(100))) {
            if (_selector.isReady(_listener)) {
                auto client = std::make_unique<sf::TcpSocket>();
                client->setBlocking(false);
                if (_listener.accept(*client) == sf::Socket::Done) {
                    std::lock_guard<std::mutex> lock(_mutex);
                    _clients.push_back(std::move(client));
                    _lastPing.push_back(std::chrono::steady_clock::now());
                    _selector.add(*_clients.back());
                    std::cout << "[TCP Server] Nouveau client → total: " << _clients.size() << std::endl;
                }
            }
            std::lock_guard<std::mutex> lock(_mutex);
            auto now = std::chrono::steady_clock::now();
            for (size_t i = 0; i < _clients.size(); ) {
                auto& socket = _clients[i];
                if (_selector.isReady(*socket)) {
                    sf::Packet packet;
                    sf::Socket::Status status = socket->receive(packet);
                    if (status == sf::Socket::Done) {
                        std::string msg;
                        if (packet >> msg && msg == "ping") {
                            sf::Packet pong;
                            pong << std::string("pong");
                            socket->send(pong);
                            _lastPing[i] = now;
                        }
                    }
                    if (status == sf::Socket::Disconnected || status == sf::Socket::Error) {
                        std::cout << "[TCP Server] Client déconnecté\n";
                        _selector.remove(*socket);
                        _clients.erase(_clients.begin() + i);
                        _lastPing.erase(_lastPing.begin() + i);
                        continue;
                    }
                }
                if (std::chrono::duration_cast<std::chrono::seconds>(now - _lastPing[i]).count() > TIMEOUT_SEC) {
                    std::cout << "[TCP Server] Timeout client → supprimé\n";
                    _selector.remove(*socket);
                    _clients.erase(_clients.begin() + i);
                    _lastPing.erase(_lastPing.begin() + i);
                } else
                    ++i;
            }
        }
    }
    _selector.clear();
}

void Nwk::TcpServer::stop()
{
    _running = false;
    _listener.close();
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& client : _clients) {
        _selector.remove(*client);
        client->disconnect();
    }
    _clients.clear();
    _lastPing.clear();
}

void Nwk::TcpServer::join()
{
    if (_thread.joinable())
        _thread.join();
}

size_t Nwk::TcpServer::getClientCount() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _clients.size();
}
