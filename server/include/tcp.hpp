/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** tcp
*/

#ifndef TCP_HPP_
    #define TCP_HPP_

    #include <vector>
    #include <poll.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <cstring>
    #include <string>
    #include <chrono>
    #include <iostream>


constexpr size_t MAX_USERS = 2;

class Server {
    public:
        explicit Server(int port);
        ~Server();

        bool init();
        int run();

    private:
        void handleClients();
        void connexion();
        void handleData(size_t id);
        void disconnectClient(size_t id);
        void ping(size_t id);
        void heartbeat();

        int _port;
        int _sockfd;
        bool _active;
        sockaddr_in _sockAddr;
        std::vector<pollfd> _fds;
        std::vector<bool> _clientActive;
        std::chrono::steady_clock::time_point _lastPing;
};


#endif /* !TCP_HPP_ */
