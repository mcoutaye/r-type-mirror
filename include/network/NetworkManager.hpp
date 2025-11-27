/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** NetworkManager
*/

#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <memory>
#include <mutex>

namespace Ntw {
    class ClientInfo {
        public:
            ClientInfo(const sf::IpAddress& ip, unsigned short port);
            ~ClientInfo() = default;
            const sf::IpAddress& getIp() const;
            unsigned short getPort() const;
            void setIp(const sf::IpAddress& ip);
            void setPort(unsigned short port);
            std::string toString() const;
        private:
            sf::IpAddress _ip;
            unsigned short _port;
    };

    class NetworkManager {
        public:
            void addClient(const sf::IpAddress& ip, unsigned short port);
            void removeClient(const sf::IpAddress& ip, unsigned short port);
            const std::vector<std::unique_ptr<ClientInfo>>& getClients() const;
            std::mutex& getClientsMutex();
        private:
            std::vector<std::unique_ptr<ClientInfo>> _clients;
            mutable std::mutex _clientsMutex;
    };
};
