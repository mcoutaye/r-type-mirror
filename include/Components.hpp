// ============================================
// 1. COMPOSANTS MANQUANTS (à ajouter dans ecs.hpp ou un Components.hpp)
// ============================================

#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// Positions et mouvements (basiques)
struct Position {
    float x = 0.f;
    float y = 0.f;
};

struct Velocity {
    float x = 0.f;
    float y = 0.f;
};

// Render
struct Drawable {
    std::string textureId;      // "ship.png", "bullet.png"
    sf::IntRect rect = {0, 0, 64, 64};  // frame actuelle
    int layer = 0;              // 0=background, 10=player, 20=bullets, 50=particles
    bool visible = true;
    float scale = 1.f;
    float rotation = 0.f;
};

// Input / Joueur
struct PlayerController {
    uint8_t playerId = 0;       // 0=joueur1, 1=joueur2...
    bool isShooting = false;
};

// Collision AABB simple
struct Collider {
    float width = 32.f;
    float height = 32.f;
    bool solid = true;          // bloquant ou pas
    uint8_t team = 0;           // 0=neutre, 1=player, 2=enemy
    int damage = 1;
};

// Health pour les ennemis/joueurs
struct Health {
    int max = 100;
    int current = 100;
};

// Pour les vagues
struct WaveSpawner {
    float nextSpawnTime = 2.f;  // temps avant prochain spawn
    int currentWave = 0;
};

struct WaveData {
    float delay;
    std::string enemyType;
    int count;
    float x, y;
};

struct MovementPattern {
    enum class Type { Linear, Sinus, Cosinus, Circle, Zigzag, Spiral };
    Type type;
    float amplitude;  // Pour les mouvements ondulatoires
    float frequency;  // Pour les mouvements ondulatoires
    float radius;     // Pour le cercle/spirale
    float speed;      // Pour le zigzag/spirale
};

struct Projectile {
    float speed;  // Vitesse du projectile
    int damage;   // Dégâts infligés
};