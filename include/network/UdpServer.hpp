/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** UdpServer
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

#define MAX_CLIENTS 4 ///< Nombre maximum de clients UDP connectés simultanément

namespace Nwk {

/**
 * @brief Informations sur un client UDP connecté
 *
 * Permet d'identifier un client par son adresse et port source,
 * et de lui associer un ID joueur unique dans la partie.
 */
class ClientInfo {
public:
    sf::IpAddress address;   ///< Adresse IP du client
    unsigned short port;     ///< Port source UDP du client
    int playerId;            ///< ID unique du joueur assigné par le serveur

    /**
     * @brief Constructeur
     * @param ip Adresse IP du client
     * @param p Port source du client
     * @param id ID joueur
     */
    ClientInfo(sf::IpAddress ip, unsigned short p, int id)
        : address(ip), port(p), playerId(id) {}
};

/**
 * @brief Serveur UDP dédié au gameplay en temps réel
 *
 * Gère :
 * - La réception des InputState des clients
 * - L'envoi de snapshots (EntityUpdate) à tous les clients connectés
 *
 * Les clients sont automatiquement ajoutés à la réception du premier paquet.
 * Utilise deux threads dédiés et des files thread-safe.
 */
class UdpServer {
public:
    /**
     * @brief Constructeur
     * @param port Port UDP sur lequel le serveur écoute (par défaut configurable)
     */
    explicit UdpServer(unsigned short port);

    /**
     * @brief Destructeur
     * Arrête les threads et libère le port UDP proprement.
     */
    ~UdpServer();

    /**
     * @brief Démarre le serveur UDP
     * Bind le socket et lance les threads de réception/envoi.
     * @return true si le bind réussit
     */
    bool start();

    /**
     * @brief Arrête le serveur
     * Ferme le socket et signale l'arrêt des threads.
     */
    void stop();

    /**
     * @brief Attend la fin des threads
     */
    void join();

    /// File contenant les inputs reçus, associés à leur playerId
    SafeQueue<std::pair<int, InputState>> receivedInputs;

    /// File contenant les paquets à envoyer (destination + données brutes)
    SafeQueue<PacketToSend> packetsToSend;

    /**
     * @brief Ajoute un client (manuellement ou auto-détection)
     * @param ip Adresse IP du client
     * @param port Port source du client
     * @param playerId ID à assigner (-1 = auto-incrément)
     * @return true si ajouté ou déjà présent
     */
    bool addClient(sf::IpAddress ip, unsigned short port, int playerId = -1);

    /**
     * @brief Supprime un client par son playerId
     * @param playerId ID du joueur à retirer
     */
    void removeClient(int playerId);

    /**
     * @brief Retourne le nombre de clients connectés
     * @return Nombre actuel de clients
     */
    std::size_t getClientCount() const { return m_clientCount; }

    /**
     * @brief Accède à la liste complète des clients (lecture seule)
     * @return Référence constante vers le vecteur de clients
     */
    const std::vector<std::unique_ptr<ClientInfo>>& getClients() const { return m_clients; }

private:
    /**
     * @brief Thread de réception des paquets UDP
     * Identifie le client, ajoute s'il est nouveau, et pousse les inputs dans receivedInputs.
     */
    void receiveThread();

    /**
     * @brief Thread d'envoi des paquets en attente
     * Consomme packetsToSend et envoie aux destinations.
     */
    void sendThread();

    /**
     * @brief Recherche un client existant par adresse et port
     * @param ip Adresse IP
     * @param port Port source
     * @return Pointeur vers ClientInfo ou nullptr
     */
    ClientInfo* findClient(const sf::IpAddress& ip, unsigned short port);

    sf::UdpSocket m_socket;                                      ///< Socket UDP principal
    unsigned short m_port;                                       ///< Port d'écoute
    std::vector<std::unique_ptr<ClientInfo>> m_clients;          ///< Liste des clients connectés
    std::size_t m_clientCount = 0;                               ///< Compteur rapide
    std::atomic<bool> m_running{false};                          ///< État d'exécution des threads
    std::thread m_recvThread;                                    ///< Thread réception
    std::thread m_sendThread;                                    ///< Thread envoi
};

} // namespace Nwk
