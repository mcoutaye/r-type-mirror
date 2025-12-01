/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** tcp
*/

#ifndef TCP_HPP_
    #define TCP_HPP_

    #include <vector>

    #ifdef _WIN32
        #include <winsock2.h>
        #include <ws2tcpip.h>
        #pragma comment(lib, "Ws2_32.lib")
        typedef WSAPOLLFD pollfd;
        #define poll WSAPoll
        using ssize_t = int;
    #else
        #include <poll.h>
        #include <netinet/in.h>
        #include <sys/socket.h>
        #include <unistd.h>
        using SOCKET = int;
        #define INVALID_SOCKET -1
        #define SOCKET_ERROR -1
        #define closesocket close
    #endif

    #include <cstring>
    #include <string>
    #include <chrono>
    #include <iostream>
    #include <thread>
    #include <atomic>


constexpr size_t MAX_USERS = 2;

class Server {
    public:
        explicit Server(int port);
        ~Server();

        bool init();

        void start();
        void stop();
        void join();

    private:
        int run();

        void handleClients();
        void connexion();
        void handleData(size_t id);
        void disconnectClient(size_t id);
        void ping(size_t id);
        void heartbeat();

        int _port;
        SOCKET _sockfd;
        std::atomic<bool> _active;

        sockaddr_in _sockAddr;
        std::vector<pollfd> _fds;
        std::vector<bool> _clientActive;

        std::chrono::steady_clock::time_point _lastPing;

        std::thread _thread;
};

#endif /* !TCP_HPP_ */
