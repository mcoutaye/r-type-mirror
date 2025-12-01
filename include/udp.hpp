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
    #include <cstdint>

    #ifdef _WIN32
        #include <winsock2.h>
        #include <ws2tcpip.h>
        #pragma comment(lib, "Ws2_32.lib")
        using ssize_t = int;
        using socklen_t = int;
    #else
        #include <sys/socket.h>
        #include <sys/select.h>
        #include <netinet/in.h>
        #include <unistd.h>
        using SOCKET = int;
        #define INVALID_SOCKET -1
        #define SOCKET_ERROR -1
        #define closesocket close
    #endif

    #include <thread>
    #include "InterThreadData.hpp"

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

        void start();
        void stop();
        void join();
        void send(std::pair<SOCKADDR_IN_T, uint8_t> data);

        ITD<SOCKADDR_IN_T> &getClientQueue() { return _clientQueue; }
        ITD<std::pair<SOCKADDR_IN_T, input_t>> _inputQueue; // for received inputs
        ITD<std::pair<SOCKADDR_IN_T, uint8_t>> _outputQueue; // for outputs to send
    private:
        uint8_t toByte(input_t input);
        input_t fromByte(uint8_t byte);

        // Client Handling
        std::array<SOCKADDR_IN_T, MAX_CLIENTS> _clients;
        ITD<SOCKADDR_IN_T> _clientQueue;
        bool isClientKnown(SOCKADDR_IN_T client);
        void addClient(SOCKADDR_IN_T client);

        // Server Networking
        SOCKET _sockfd;
        PORT _port;
        SOCKADDR_IN_T _serverAddr;
        std::atomic<bool> _active;

        // Sending thread
        int sender(); // Starting function
        std::thread _threadSend;
        uint8_t _buffer[MAX_INCOMING_UDP_SIZE];

        // Receiving thread
        int receiver(); // Starting function
        std::thread _threadReceive;

};

#endif /* !UDP_HPP_ */
