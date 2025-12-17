/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** TcpClient
*/

#include "network/TcpClient.hpp"

Nwk::TcpClient::TcpClient(const std::string& ip, unsigned short port)
    : _ip(ip), _port(port), _socket(std::make_unique<sf::TcpSocket>())
{
}

Nwk::TcpClient::~TcpClient()
{
    stop();
    join();
    if (_socket && _socket->getRemotePort() != 0) {
        _socket->disconnect();
        std::cout << "[TCP Client] Déconnecté proprement (destructeur)\n";
    }
}

bool Nwk::TcpClient::start()
{
    if (_running)
        return false;
    if (_socket->connect(_ip, _port) != sf::Socket::Done) {
        std::cerr << "[TCP Client] Connexion échouée\n";
        return false;
    }
    _socket->setBlocking(false);
    auto now = std::chrono::steady_clock::now();
    _lastPingSent = now;
    _lastPongReceived = now;
    _connected = true;
    _running = true;
    _thread = std::thread(&Nwk::TcpClient::run, this);
    std::cout << "[TCP Client] Connecté – envoi ping toutes les secondes\n";
    return true;
}

void Nwk::TcpClient::run() {
    sf::SocketSelector selector;
    selector.add(*_socket);
    while (_running) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastPingSent).count() >= 1000) {
            sf::Packet ping;
            ping << std::string("ping");
            _socket->send(ping);
            _lastPingSent = now;
        }
        if (selector.wait(sf::milliseconds(100))) {
            if (selector.isReady(*_socket)) {
                sf::Packet packet;
                if (_socket->receive(packet) == sf::Socket::Done) {
                    std::string msg;
                    if (packet >> msg && msg == "pong")
                        _lastPongReceived = now;
                } else
                    break;
            }
        }
        if (std::chrono::duration_cast<std::chrono::seconds>(now - _lastPongReceived).count() > TIMEOUT_SEC) {
            std::cout << "[TCP Client] Timeout serveur → arrêt\n";
            break;
        }
    }
    _socket->disconnect();
    _connected = false;
}

void Nwk::TcpClient::stop() { _running = false; }

void Nwk::TcpClient::join()
{
    if (_thread.joinable())
        _thread.join();
}