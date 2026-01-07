/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Components hpp
*/

#pragma once
#include <SFML/Graphics.hpp>
#include <string>

#define SHOOT_DELAY 0.5f

// Should send updates to clients or not
typedef struct SendUpdate_s {
    bool needsUpdate = false;
} SendUpdate_t;

// Positions et mouvements (basiques)
typedef struct Position_s {
    float x = 0.f;
    float y = 0.f;
} Position_t;

typedef struct Velocity_s {
    float x = 0.f;
    float y = 0.f;
} Velocity_t;

// Render
typedef struct Drawable_s {
    char textureId[64];      // "ship.png", "bullet.png"
    sf::IntRect rect = {0, 0, 64, 64};  // frame actuelle
    int layer = 0;              // 0=background, 10=player, 20=bullets, 50=particles
    bool visible = true;
    float scale = 1.f;
    float rotation = 0.f;
} Drawable_t;

// Input / Joueur
typedef struct PlayerController_s {
    uint8_t playerId = 0;       // 0=joueur1, 1=joueur2...
    bool isShooting = false;
    float shootCooldown = 0.f;
} PlayerController_t;

// Collision AABB simple
typedef struct Collider_s {
    float width = 32.f;
    float height = 32.f;
    bool solid = true;          // bloquant ou pas
    uint8_t team = 0;           // 0=neutre, 1=player, 2=enemy
    int damage = 1;
} Collider_t;

// Health pour les ennemis/joueurs
typedef struct Health_s {
    int max = 100;
    int current = 100;
    int lastAttackerId = -1;
} Health_t;

// Pour les vagues
typedef struct WaveSpawner_s {
    float nextSpawnTime = 2.f;  // temps avant prochain spawn
    int currentWave = 0;
} WaveSpawner_t;

typedef struct WaveData_s {
    float delay;
    char enemyType[16];
    int count;
    float x, y;
} WaveData_t;

typedef struct MovementPattern_s {
    enum class Type { Linear, Sinus, Cosinus, Circle, Zigzag, Spiral };
    Type type;
    float amplitude;  // Pour les mouvements ondulatoires
    float frequency;  // Pour les mouvements ondulatoires
    float radius;     // Pour le cercle/spirale
    float speed;      // Pour le zigzag/spirale
} MovementPattern_t;

typedef struct Projectile_s {
    float speed;  // Vitesse du projectile
    int damage;   // Dégâts infligés
    int ownerId = -1;
} Projectile_t;

// Obstacle de stage indestructible
typedef struct Obstacle_s {
    bool blocking = true;  // Bloque le mouvement
} Obstacle_t;

// Étoile du fond (background starfield)
typedef struct Star_s {
    float speed = 100.f;      // Vitesse de défilement
    uint8_t brightness = 255; // Luminosité (pour effet de parallaxe)
    uint8_t size = 2;         // Taille de l'étoile
} Star_t;

typedef struct JustShot_s {
    bool active = true;
} JustShot_t;

// Trigger pour jouer un effet sonore une seule fois
typedef struct PlaySound_s {
    char soundId[64];      // ex: "player_shoot.wav", "enemy_explosion.wav", "hit.wav"
    float volume = 100.f;  // 0-100 (SFML utilise 0-100)
    float pitch = 1.0f;     // variation de tonalité
} PlaySound_t;

// Musique de fond (une seule à la fois généralement)
typedef struct BackgroundMusic_s {
    char musicId[64];      // ex: "level1.ogg", "menu_theme.ogg"
    bool looping = true;
    float volume = 50.f;
} BackgroundMusic_t;
