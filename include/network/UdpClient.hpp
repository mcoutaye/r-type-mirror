/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** UdpClient
*/

#pragma once

#include "SafeQueue.hpp"
#include <SFML/Network.hpp>
#include "NetworkProtocol.hpp"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <iostream>
#include <sstream>
#include <cstring>

#define SERVER_PORT 53000 ///< Port UDP par défaut du serveur

namespace Nwk {

/**
 * @brief Client UDP dédié au gameplay en temps réel
 *
 * Responsable de :
 * - L'envoi fréquent des inputs du joueur vers le serveur
 * - La réception des snapshots d'entités (positions de toutes les entités du jeu)
 *
 * Fonctionne avec deux threads indépendants et utilise des files thread-safe
 * pour communiquer avec la logique du jeu.
 */
class UdpClient {
public:
    /**
     * @brief Constructeur
     * @param serverIp Adresse IP du serveur UDP (défaut : localhost)
     * @param serverPort Port UDP du serveur (défaut : 53000)
     */
    explicit UdpClient(sf::IpAddress serverIp = "127.0.0.1", unsigned short serverPort = SERVER_PORT);

    /**
     * @brief Destructeur
     * Arrête proprement les threads et libère le port local.
     */
    ~UdpClient();

    /**
     * @brief Démarre le client UDP
     * Bind un port local et lance les threads d'envoi et de réception.
     * @param localPort Port local à binder (0 = port éphémère choisi par l'OS)
     * @return true si le bind a réussi, false sinon
     */
    bool start(unsigned short localPort = 0);

    /**
     * @brief Arrête les threads et libère le socket
     */
    void stop();

    /**
     * @brief Attend la terminaison des threads d'envoi et de réception
     */
    void join();

    /// File thread-safe contenant les inputs locaux à envoyer au serveur
    SafeQueue<InputState> inputsToSend;

    /// File thread-safe contenant les snapshots d'entités reçus du serveur
    SafeQueue<std::vector<EntityUpdate>> receivedUpdates;

private:
    /**
     * @brief Thread dédié à l'envoi des inputs
     * Consomme inputsToSend et envoie chaque InputState individuellement au serveur.
     */
    void sendThread();

    /**
     * @brief Thread dédié à la réception des snapshots
     * Reçoit les paquets UDP, vérifie leur taille et les découpe en EntityUpdate.
     */
    void receiveThread();

    sf::UdpSocket m_socket;              ///< Socket UDP local bindé
    sf::IpAddress m_serverIp;            ///< Adresse IP du serveur
    unsigned short m_serverPort;         ///< Port UDP du serveur
    std::atomic<bool> m_running{false};  ///< Indicateur d'exécution des threads
    std::thread m_sendThread;            ///< Thread responsable de l'envoi
    std::thread m_recvThread;            ///< Thread responsable de la réception
};

} // namespace Nwk
