/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** TcpServer
*/

#pragma once

#include <SFML/Network.hpp>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <memory>
#include <iostream>

namespace Nwk {

/**
 * @brief Serveur TCP pour la gestion des connexions et keep-alive
 *
 * Fonctionnalités :
 * - Acceptation de nouvelles connexions clientes
 * - Réponse aux ping avec pong
 * - Détection et suppression des clients inactifs (timeout)
 *
 * Utilise un SocketSelector pour gérer plusieurs clients dans un seul thread.
 */
class TcpServer {
public:
    /**
     * @brief Constructeur
     * @param port Port TCP d'écoute (défaut : 53001)
     */
    TcpServer(unsigned short port = 53001);

    /**
     * @brief Destructeur
     * Arrête le serveur et déconnecte tous les clients.
     */
    ~TcpServer();

    /**
     * @brief Démarre le serveur TCP
     * @return true si l'écoute a réussi
     */
    bool start();

    /**
     * @brief Arrête le serveur et déconnecte tous les clients
     */
    void stop();

    /**
     * @brief Attend la fin du thread principal
     */
    void join();

    /**
     * @brief Retourne le nombre de clients connectés
     * @return Nombre actuel de clients
     */
    size_t getClientCount() const;

private:
    /**
     * @brief Boucle principale du serveur
     * Gère les nouvelles connexions, ping/pong et timeouts.
     */
    void run();

    unsigned short _port;                                        ///< Port d'écoute TCP
    sf::TcpListener _listener;                                   ///< Listener TCP
    std::vector<std::unique_ptr<sf::TcpSocket>> _clients;        ///< Sockets des clients
    std::vector<std::chrono::steady_clock::time_point> _lastPing; ///< Dernier ping reçu par client
    sf::SocketSelector _selector;                                ///< Sélecteur d'événements
    std::thread _thread;                                         ///< Thread principal
    std::atomic<bool> _running{false};                           ///< État d'exécution
    mutable std::mutex _mutex;                                   ///< Protection des collections

    static constexpr int TIMEOUT_SEC = 5; ///< Timeout avant déconnexion automatique (5s)
};

} // namespace Nwk
