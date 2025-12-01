/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** udp
*/

#include "udp.hpp"

#ifndef _WIN32
    #include <arpa/inet.h>
#endif

UDP::UDP(int port)
{
    std::memset(&_buffer, 0, sizeof(_buffer));
    _port = port;
    for (auto &client : _clients) {
        std::memset(&client, 0, sizeof(SOCKADDR_IN_T));
    }
    _sockfd = INVALID_SOCKET;
}

UDP::~UDP()
{
    if (_sockfd != INVALID_SOCKET)
        closesocket(_sockfd);

    #ifdef _WIN32
        WSACleanup();
    #endif
}

bool UDP::init()
{
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
            return false;
        }
    #endif

    if ( (_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET ) {
        std::cerr << "Error opening UDP socket" << std::endl;
        return false;
    }
    std::memset(&_serverAddr, 0, sizeof(_serverAddr));
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    _serverAddr.sin_port = htons(_port);
    if (bind(_sockfd, reinterpret_cast<struct sockaddr *>(&_serverAddr), sizeof(_serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error binding UDP socket" << std::endl;
        return false;
    }
    std::cout << "UDP server listening on port " << _port << std::endl;
    return true;
}

int UDP::receiver()
{
    socklen_t len;
    ssize_t n;
    SOCKADDR_IN_T clientAddr;
    fd_set readfds;
    struct timeval tv;

    while (_active) {

        // Set up select to wait for data with a timeout in case we end the server
        FD_ZERO(&readfds);
        FD_SET(_sockfd, &readfds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // 100ms
        int ret = select(_sockfd + 1, &readfds, nullptr, nullptr, &tv);
        if (ret < 0) {
            if (errno == EINTR) continue;
            std::cerr << "Error in select" << std::endl;
            break;
        }
        if (ret == 0) {
            continue; // Timeout, check _active again
        }

        len = sizeof(SOCKADDR_IN_T);
        std::memset(&clientAddr, 0, sizeof(clientAddr));
        n = recvfrom(_sockfd, reinterpret_cast<uint8_t *>(_buffer), MAX_INCOMING_UDP_SIZE, MSG_WAITALL,
                     reinterpret_cast<struct sockaddr *>(&clientAddr), &len);
        if (n < 0) {
            std::cerr << "Error receiving UDP data" << std::endl;
            continue;
        }

        if (!isClientKnown(clientAddr)) // Adds a client if just received data for the first time
            addClient(clientAddr);

        if (n == sizeof(uint8_t)) {
            input_t input = fromByte(static_cast<uint8_t>(_buffer[0]));
            // std::cout << "Received input: "
            //           << "Up: " << static_cast<bool>(input.up) << ", "
            //           << "Down: " << static_cast<bool>(input.down) << ", "
            //           << "Left: " << static_cast<bool>(input.left) << ", "
            //           << "Right: " << static_cast<bool>(input.right) << ", "
            //           << "Shoot: " << static_cast<bool>(input.shoot) << std::endl;
            _inputQueue.Push(std::make_pair(clientAddr, input));
        }
    }
    return 0;
}

int UDP::sender()
{
    std::pair<SOCKADDR_IN_T, uint8_t> dataToSend;

    while (_active) {
        _outputQueue.Pop(dataToSend); // Thread sleeps here until there's data to send
        if (!_active)
            break;
        send(dataToSend);
    }
    return 0;
}

uint8_t UDP::toByte(input_t input)
{
    uint8_t byte = 0;
    byte |= (input.up & 0x01) << 0;
    byte |= (input.down & 0x01) << 1;
    byte |= (input.left & 0x01) << 2;
    byte |= (input.right & 0x01) << 3;
    byte |= (input.shoot & 0x01) << 4;
    return byte;
}

input_t UDP::fromByte(uint8_t byte)
{
    input_t input;
    input.up = (byte >> 0) & 0x01;
    input.down = (byte >> 1) & 0x01;
    input.left = (byte >> 2) & 0x01;
    input.right = (byte >> 3) & 0x01;
    input.shoot = (byte >> 4) & 0x01;
    return input;
}

bool UDP::isClientKnown(SOCKADDR_IN_T client)
{
    for (const auto &known : _clients) {
        if (known.sin_addr.s_addr == client.sin_addr.s_addr &&
            known.sin_port == client.sin_port) {
            return true;
        }
    }
    return false;
}

void UDP::addClient(SOCKADDR_IN_T client)
{
    for (auto &known : _clients) {
        if (known.sin_addr.s_addr == 0 && known.sin_port == 0) {
            known = client;
            _clientQueue.Push(client);
            std::cout << "New client connected: " << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << std::endl;
            return;
        }
    }
}

void UDP::start()
{
    _active = true;
    _threadReceive = std::thread([this] {
        receiver();
    });
    _threadSend = std::thread([this] {
        sender();
    });
}

void UDP::stop()
{
    std::cout << "Stopping UDP server..." << std::endl;
    _active = false;
    SOCKADDR_IN_T dummy = {0};
    _outputQueue.Push(std::make_pair(dummy, 0));
}

void UDP::join()
{
    if (_threadReceive.joinable())
        _threadReceive.join();
    if (_threadSend.joinable())
        _threadSend.join();
}

void UDP::send(std::pair<SOCKADDR_IN_T, uint8_t> data)
{
    const SOCKADDR_IN_T &clientAddr = data.first;
    uint8_t byte = data.second;

    ssize_t n = sendto(_sockfd, reinterpret_cast<uint8_t *>(&byte), sizeof(byte), 0,
                       reinterpret_cast<const struct sockaddr *>(&clientAddr), sizeof(clientAddr));
    if (n == SOCKET_ERROR) {
        std::cerr << "Error sending UDP data to "
                  << inet_ntoa(clientAddr.sin_addr) << ":"
                  << ntohs(clientAddr.sin_port) << std::endl;
    } else {
        std::cout << "Sent data to "
                  << inet_ntoa(clientAddr.sin_addr) << ":"
                  << ntohs(clientAddr.sin_port) << std::endl;
    }
}