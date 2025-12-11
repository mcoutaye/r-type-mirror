/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Components hpp
*/

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

struct Shootable {
    float cooldown;      // Temps restant avant le prochain tir
    float shootDelay;    // Délai entre deux tirs
    float missileSpeed;  // Vitesse des projectiles
    int damage;          // Dégâts infligés
    uint8_t team;             // Équipe du tireur (pour éviter les tirs alliés)
    std::string textureId; // Texture du projectile
    float offsetX;        // Offset X pour le spawn du projectile
    float offsetY;        // Offset Y pour le spawn du projectile
    bool isShooting;  // Nouveau champ pour indiquer si le joueur tire
    bool tripleShot;
};

struct PowerUp {
    enum class Type { TripleShot };
    Type type;
    float duration;  // Durée de l'effet (en secondes)
};

// Obstacle de stage indestructible
struct Obstacle {
    bool blocking = true;  // Bloque le mouvement
};

// Tuile destructible
struct DestructibleTile {
    int health = 50;  // Points de vie de la tuile
};

// Étoile du fond (background starfield)
struct Star {
    float speed = 100.f;      // Vitesse de défilement
    uint8_t brightness = 255; // Luminosité (pour effet de parallaxe)
    uint8_t size = 2;         // Taille de l'étoile
};
