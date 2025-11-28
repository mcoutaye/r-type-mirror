/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** tcp
*/

#include "tcp.hpp"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

Server::Server(int port) : _port(port), _sockfd(INVALID_SOCKET), _active(false)
{
    std::memset(&_sockAddr, 0, sizeof(_sockAddr));
}

Server::~Server()
{
    for (auto &fd : _fds) {
        if (fd.fd != INVALID_SOCKET)
            closesocket(fd.fd);
    }

    if (_sockfd != INVALID_SOCKET)
        closesocket(_sockfd);

    #ifdef _WIN32
        WSACleanup();
    #endif
}

bool Server::init()
{
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
            return false;
        }
    #endif

    // Initialize socket
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == INVALID_SOCKET) {
        std::cerr << "Error opening socket" << std::endl;
        return false;
    }

    // Setup sockaddr_in structure
    _sockAddr.sin_family = AF_INET;
    _sockAddr.sin_addr.s_addr = INADDR_ANY;
    _sockAddr.sin_port = htons(_port);

    // Bind socket
    if (bind(_sockfd, reinterpret_cast<struct sockaddr *>(&_sockAddr), sizeof(_sockAddr)) == SOCKET_ERROR) {
        std::cerr << "Error on binding" << std::endl;
        closesocket(_sockfd);
        return false;
    }

    // Listen for incoming connections
    if (listen(_sockfd, MAX_USERS) == SOCKET_ERROR) {
        std::cerr << "Error on listen" << std::endl;
        closesocket(_sockfd);
        return false;
    }

    std::cout << "Server listening on port " << _port << std::endl;

    _fds.resize(MAX_USERS + 1);
    _clientActive.resize(MAX_USERS + 1, false);
    for (auto &fd : _fds) {
        fd.fd = INVALID_SOCKET;
        fd.events = POLLIN;
        fd.revents = 0;
    }
    _fds[0].fd = _sockfd;
    _fds[0].events = POLLIN;
    _active = true;
    return true;
}

int Server::run()
{
    if (!_active) {
        std::cerr << "Server is not active. Call init() first." << std::endl;
        return -1;
    }
    while (_active) {
        int pollCount = poll(_fds.data(), static_cast<unsigned long>(_fds.size()), 0);
        if (pollCount == SOCKET_ERROR) {
            std::cerr << "Error on poll" << std::endl;
            return 84;
        }
        handleClients();
        heartbeat();
    }
    return 0;
}

void Server::start()
{
    _active = true;
    _thread = std::thread([this] {
        run();
    });
}

void Server::stop()
{
    std::cout << "Stopping server..." << std::endl;
    _active = false;
}

void Server::join()
{
    if (_thread.joinable())
        _thread.join();
}

void Server::handleClients()
{
    for (size_t i = 0; i < _fds.size(); ++i) {
        if (_fds[i].revents & POLLIN) {
            if (_fds[i].fd == INVALID_SOCKET)
                continue;
            if (i == 0)
                connexion();
            else
                handleData(i);
        }
    }
}

void Server::connexion()
{
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    SOCKET newfd = accept(_sockfd, reinterpret_cast<struct sockaddr *>(&clientAddr), &clientLen);

    if (newfd == INVALID_SOCKET) {
        std::cerr << "Error on accept" << std::endl;
        return;
    }

    // Add new client to _fds
    for (size_t i = 1; i < _fds.size(); i++) {
        if (_fds[i].fd == INVALID_SOCKET) {
            _fds[i].fd = newfd;
            _clientActive[i] = true;
            std::cout << "New client connected: FD " << newfd << std::endl;
            return;
        }
    }
    std::cerr << "Max clients reached. Connection refused." << std::endl;
    closesocket(newfd);
}

void Server::handleData(size_t id)
{
    char buffer[1024];
    ssize_t bytesRead = 0;

    memset(buffer, 0, sizeof(buffer));
    bytesRead = recv(_fds[id].fd, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead <= 0) {
        std::cout << "Client fd " << _fds[id].fd << " closed connection." << std::endl;
        disconnectClient(id);
        return;
    }

    buffer[bytesRead] = '\0';
    std::cout << "Received data from FD " << _fds[id].fd << ": " << buffer << std::endl;

    // Check heartbeat response
    if (std::strncmp(buffer, "PONG", 4) == 0) {
        _clientActive[id] = true;
        std::cout << "Received PONG from FD " << _fds[id].fd << std::endl;
        return;
    }
}

void Server::disconnectClient(size_t id)
{
    std::string bye = "DISCONNECTED\n\n";

    send(_fds[id].fd, bye.c_str(), static_cast<int>(bye.size()), 0);
    std::cout << "Disconnecting client FD " << _fds[id].fd << std::endl;
    closesocket(_fds[id].fd);
    _fds[id].fd = INVALID_SOCKET;
    _fds[id].revents = 0;
}

void Server::ping(size_t id)
{
    const std::string pingMessage = "PING\n";
    ssize_t bytesSent = send(_fds[id].fd, pingMessage.c_str(), static_cast<int>(pingMessage.size()), 0);

    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Error sending PING to FD " << _fds[id].fd << std::endl;
        disconnectClient(id);
    }
}

void Server::heartbeat()
{
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - _lastPing).count();

    if (elapsed < 5)
        return;

    std::cout << "[HEARTBEAT] Checking clients..." << std::endl;
    _lastPing = now;

    for (size_t i = 1; i < _fds.size(); i++) {
        if (_fds[i].fd != INVALID_SOCKET) {
            if (!_clientActive[i]) {
                std::cout << "No PONG from FD " << _fds[i].fd << ". Disconnecting." << std::endl;
                disconnectClient(i);
            } else {
                _clientActive[i] = false;
                ping(i);
            }
        }
    }
}
