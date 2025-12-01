/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** udp
*/

#include "udp.hpp"

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

int UDP::run()
{
    socklen_t len;
    ssize_t n;

    len = sizeof(SOCKADDR_IN_T);
    _active = true;
    while (_active) {
        n = recvfrom(_sockfd, _buffer, MAX_INCOMING_UDP_SIZE, MSG_WAITALL,
                     reinterpret_cast<struct sockaddr *>(&_serverAddr), &len);
        if (n < 0) {
            std::cerr << "Error receiving UDP data" << std::endl;
            continue;
        }

        if (n == sizeof(uint8_t)) {
            input_t input = fromByte(static_cast<uint8_t>(_buffer[0]));
            std::cout << "Received input: "
                      << "Up: " << static_cast<bool>(input.up) << ", "
                      << "Down: " << static_cast<bool>(input.down) << ", "
                      << "Left: " << static_cast<bool>(input.left) << ", "
                      << "Right: " << static_cast<bool>(input.right) << ", "
                      << "Shoot: " << static_cast<bool>(input.shoot) << std::endl;
            continue;
        }

        _buffer[n] = '\0';
        if (std::strncmp(_buffer, "STOP" , 4) == 0) {
            std::cout << "Received STOP command. Shutting down UDP server." << std::endl;
            _active = false;
            break;
        }
        std::cout << "Received UDP data: " << _buffer << std::endl;
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
