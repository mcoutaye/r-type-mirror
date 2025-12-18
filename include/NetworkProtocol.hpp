// include/NetworkProtocol.hpp
#pragma once
#include <cstdint>
#include <vector>
#include "engine/systems/Components.hpp"

// Ensure packing is forced on all platforms
#pragma pack(push, 1)

#ifdef _MSC_VER
    #define PACKED
#else
    #define PACKED __attribute__((packed))
#endif

// === INPUT CLIENT → SERVEUR ===
struct PACKED InputState {
    uint8_t up    : 1;
    uint8_t down  : 1;
    uint8_t left  : 1;
    uint8_t right : 1;
    uint8_t shoot : 1;
    uint8_t       : 3;
    uint16_t tick; // Wraps around 65535
};
static_assert(sizeof(InputState) == 3, "InputState must be 3 bytes");

// Magic numbers for tick
#define MAGIC_TICK_SHOOT_PLAYER 0xFFFB
#define MAGIC_TICK_SHOOT_ENEMY  0xFFFC
#define MAGIC_TICK_CLIENT_QUIT  0xFFFA // Client wants to quit

#define MAGIC_TICK_DEATH_PLAYER 0xFFFD // You killed it
#define MAGIC_TICK_DEATH_OTHER  0xFFFE // It died
#define MAGIC_TICK_LOCAL_PLAYER 0xFFFF // This is you

// Helper to check if 'newTick' is more recent than 'oldTick' handling wrap-around
inline bool IsTickNewer(uint16_t newTick, uint16_t oldTick) {
    // If the difference is small (e.g., < 32768), it's a normal increase.
    // If the difference is huge (e.g., new=5, old=65530), it's a wrap-around.
    return (newTick != oldTick) && ((newTick - oldTick) < 32768);
}


// === UPDATE SERVEUR → CLIENT (ECS positions) ===
struct PACKED EntityUpdate {
    uint32_t entityId;  // 4 bytes
    uint16_t tick;      // 2 bytes

    struct PACKED { // position_t
        float x;
        float y;
    } position;         // 8 bytes

    struct PACKED { // health_t
        int current;
        int max;
    } health;           // 8 bytes

};
static_assert(sizeof(EntityUpdate) == 22, "EntityUpdate must be 22 bytes");

#pragma pack(pop)
#undef PACKED

// === PAQUET À ENVOYER (serveur uniquement) ===
struct PacketToSend {
    sf::IpAddress          destIp;
    unsigned short         destPort;
    std::vector<uint8_t>   data;
};
