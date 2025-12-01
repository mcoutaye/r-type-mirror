/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** udp
*/

#ifndef UDP_HPP_
    #define UDP_HPP_

    #include <array>
    #include <iostream>
    #include <atomic>
    #include <cstring>

    #ifdef _WIN32
        #include <winsock2.h>
        #include <ws2tcpip.h>
        #pragma comment(lib, "Ws2_32.lib")
        using ssize_t = int;
        using socklen_t = int;
    #else
        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <unistd.h>
        using SOCKET = int;
        #define INVALID_SOCKET -1
        #define SOCKET_ERROR -1
        #define closesocket close
    #endif

#define MAX_INCOMING_UDP_SIZE 1 << 10
#define MAX_CLIENTS 4

typedef struct sockaddr_in SOCKADDR_IN_T;

using PORT = uint16_t;

typedef struct input_s {
    uint8_t up : 1;
    uint8_t down : 1;
    uint8_t left : 1;
    uint8_t right : 1;
    uint8_t shoot : 1;
} input_t;

class UDP
{
    public:
        explicit UDP(int port);
        ~UDP();

        bool init();
        int run();

        uint8_t toByte(input_t input);

        input_t fromByte(uint8_t byte);

        // void start();
        // void stop();
        // void join();

    private:
        SOCKET _sockfd;
        PORT _port;
        char _buffer[MAX_INCOMING_UDP_SIZE];
        std::array<SOCKADDR_IN_T, MAX_CLIENTS> _clients;
        SOCKADDR_IN_T _serverAddr;
        std::atomic<bool> _active;
};

#endif /* !UDP_HPP_ */
