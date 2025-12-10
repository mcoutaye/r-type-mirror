// include/NetworkProtocol.hpp
#pragma once
#include <cstdint>
#include <vector>
#include "engine/systems/Components.hpp"

// === INPUT CLIENT → SERVEUR ===
struct InputState {
    uint8_t up    : 1;
    uint8_t down  : 1;
    uint8_t left  : 1;
    uint8_t right : 1;
    uint8_t shoot : 1;
    uint8_t       : 3;
    uint16_t tick; // Wraps around 65535
} __attribute__((packed));
static_assert(sizeof(InputState) == 3, "InputState must be 3 bytes");

// Helper to check if 'newTick' is more recent than 'oldTick' handling wrap-around
inline bool IsTickNewer(uint16_t newTick, uint16_t oldTick) {
    // If the difference is small (e.g., < 32768), it's a normal increase.
    // If the difference is huge (e.g., new=5, old=65530), it's a wrap-around.
    return (newTick != oldTick) && ((newTick - oldTick) < 32768);
}


// === UPDATE SERVEUR → CLIENT (ECS positions) ===
struct __attribute__((packed)) EntityUpdate {
    uint32_t entityId;  // 4 bytes
    uint16_t tick;      // 2 bytes

    struct __attribute__((packed)) { // position_t
        float x;
        float y;
    } position;         // 8 bytes

    struct __attribute__((packed)) { // health_t
        int current;
        int max;
    } health;           // 8 bytes

}__attribute__((packed));
static_assert(sizeof(EntityUpdate) == 22, "EntityUpdate must be 20 bytes");

// === PAQUET À ENVOYER (serveur uniquement) ===
struct PacketToSend {
    sf::IpAddress          destIp;
    unsigned short         destPort;
    std::vector<uint8_t>   data;
};
