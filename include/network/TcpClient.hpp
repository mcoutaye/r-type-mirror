/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** TcpClient
*/

#pragma once

#include <SFML/Network.hpp>
#include <thread>
#include <atomic>
#include <chrono>
#include <string>
#include <memory>
#include <iostream>

namespace Nwk {

/**
 * @brief Client TCP pour l'établissement de connexion et keep-alive
 *
 * Responsable de :
 * - Connexion initiale au serveur TCP
 * - Envoi régulier de "ping"
 * - Réception de "pong" pour détecter les déconnexions
 *
 * Utilisé en parallèle du canal UDP pour le gameplay.
 */
class TcpClient {
public:
    /**
     * @brief Constructeur
     * @param ip Adresse IP du serveur TCP (défaut : localhost)
     * @param port Port TCP du serveur (défaut : 53001)
     */
    explicit TcpClient(const std::string& ip = "127.0.0.1", unsigned short port = 53001);

    /**
     * @brief Destructeur
     * Déconnecte proprement si encore connecté.
     */
    ~TcpClient();

    /**
     * @brief Établit la connexion TCP et démarre le thread de gestion
     * @return true si la connexion a réussi
     */
    bool start();

    /**
     * @brief Arrête le thread de gestion
     */
    void stop();

    /**
     * @brief Attend la fin du thread principal
     */
    void join();

    /**
     * @brief Vérifie si la connexion est active (pong reçu récemment)
     * @return true si connecté et keep-alive valide
     */
    bool isConnected() const { return _connected.load(); }

private:
    /**
     * @brief Boucle principale du client
     * Gère l'envoi des ping et la surveillance des pong.
     */
    void run();

    std::string _ip;                                             ///< IP du serveur
    unsigned short _port;                                        ///< Port du serveur
    std::unique_ptr<sf::TcpSocket> _socket;                       ///< Socket TCP
    std::thread _thread;                                         ///< Thread de gestion
    std::atomic<bool> _running{false};                           ///< État d'exécution
    std::atomic<bool> _connected{false};                         ///< Connexion valide
    std::chrono::steady_clock::time_point _lastPingSent;         ///< Dernier ping envoyé
    std::chrono::steady_clock::time_point _lastPongReceived;     ///< Dernier pong reçu

    static constexpr int PING_INTERVAL_MS = 1000; ///< Intervalle entre deux ping (1s)
    static constexpr int TIMEOUT_SEC = 5;         ///< Timeout avant détection de déconnexion
};

} // namespace Nwk
