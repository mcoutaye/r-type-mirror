// include/NetworkProtocol.hpp
#pragma once
#include <cstdint>
#include <vector>

// === INPUT CLIENT → SERVEUR ===
struct InputState {
    uint8_t up    : 1;
    uint8_t down  : 1;
    uint8_t left  : 1;
    uint8_t right : 1;
    uint8_t shoot : 1;
    uint8_t       : 3;
};
static_assert(sizeof(InputState) == 1, "InputState must be 1 byte");

// === UPDATE SERVEUR → CLIENT (ECS positions) ===
struct EntityUpdate {
    uint32_t entityId;
    float    x;
    float    y;
    // Ajoute ici : velocity, rotation, type, etc. si besoin
};
static_assert(sizeof(EntityUpdate) == 12, "EntityUpdate must be 12 bytes");

// === PAQUET À ENVOYER (serveur uniquement) ===
struct PacketToSend {
    sf::IpAddress          destIp;
    unsigned short         destPort;
    std::vector<uint8_t>   data;
};
