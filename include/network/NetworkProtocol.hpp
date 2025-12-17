/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** NetworkProtocol
*/

#pragma once

#include <cstdint>
#include <vector>
#include <SFML/Network.hpp>  // Pour sf::IpAddress

namespace Nwk {

/**
 * @brief État des inputs du joueur (1 octet)
 *
 * Structure envoyée fréquemment du client UDP vers le serveur
 * pour transmettre les commandes du joueur en temps réel.
 */
struct InputState {
    uint8_t up    : 1; ///< Bouton haut appuyé
    uint8_t down  : 1; ///< Bouton bas appuyé
    uint8_t left  : 1; ///< Bouton gauche appuyé
    uint8_t right : 1; ///< Bouton droit appuyé
    uint8_t shoot : 1; ///< Tir appuyé
    uint8_t       : 3; ///< Bits réservés (padding)
};

static_assert(sizeof(InputState) == 1, "InputState must be 1 byte");

/**
 * @brief Mise à jour minimale d'une entité du jeu (12 octets)
 *
 * Envoyée par le serveur UDP à tous les clients pour synchroniser
 * les positions des entités (joueurs, ennemis, projectiles, etc.).
 */
struct EntityUpdate {
    uint32_t entityId; ///< Identifiant unique de l'entité
    float x;           ///< Position horizontale
    float y;           ///< Position verticale
};

static_assert(sizeof(EntityUpdate) == 12, "EntityUpdate must be 12 bytes");

/**
 * @brief Structure pour mettre en file d'attente un paquet UDP à envoyer
 *
 * Utilisée par UdpServer pour stocker les snapshots d'entités
 * avec leur destination (IP + port du client).
 */
struct PacketToSend {
    sf::IpAddress destIp;          ///< Adresse IP de destination
    unsigned short destPort;       ///< Port UDP de destination
    std::vector<uint8_t> data;     ///< Données brutes (ex: plusieurs EntityUpdate concatenés)
};

} // namespace Nwk
