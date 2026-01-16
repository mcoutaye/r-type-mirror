/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Components hpp
*/

#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "engine/Menu.hpp"
#include <string>
#include <functional>

#define SHOOT_DELAY 0.5f

// Forward declaration / enum pour les actions de jeu
enum class GameAction {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Shoot,
    Quit,
    Pause,      // Toggle pause/resume en jeu
    MenuUp,
    MenuDown,
    MenuSelect
};

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

// Texte simple pour les items du menu (titre, boutons...)
typedef struct Text_s {
    char text[128];
    char fontId[32] = "default";
    uint32_t fontSize = 48;
    sf::Color color;
    sf::Color originalColor;  // Ajout du champ pour sauvegarder la couleur originale
    bool centered = true;
    bool visible = true;
} Text_t;

// Indique que c'est un item de menu sélectionnable
typedef struct MenuItem_s {
    MenuAction action;
    bool isSelected = false;
    bool isSelectable = true;  // Peut être sélectionné par la navigation clavier/souris
} MenuItem_t;

// Optionnel : pour highlight visuel (ex: changer couleur ou scale quand sélectionné)
typedef struct Highlight_s {
    sf::Color selectedColor = sf::Color::Yellow;
    float selectedScale = 1.2f;
} Highlight_t;

// Slider pour contrôler des valeurs (volume, sensibilité, etc.)
typedef struct Slider_s {
    float minValue = 0.0f;
    float maxValue = 100.0f;
    float currentValue = 50.0f;
    float step = 1.0f;
    std::string linkedSetting;  // Ex: "music_volume", "sfx_volume"
} Slider_t;

// Type d'input pour le remapping
enum class InputType {
    Keyboard,
    Joystick
};

// Bouton pour remapper une touche ou un bouton de manette
typedef struct KeybindButton_s {
    GameAction action;  // L'action à remapper
    InputType inputType = InputType::Keyboard;  // Type d'input (clavier ou manette)
    bool isWaitingForInput = false;  // En attente d'une nouvelle touche/bouton
    sf::Keyboard::Key currentKey = sf::Keyboard::Unknown;
    unsigned int currentJoystickButton = static_cast<unsigned int>(-1);
} KeybindButton_t;

// Texte dynamique qui affiche une valeur (pour les sliders ou keybinds)
typedef struct DynamicText_s {
    std::string prefix;  // Ex: "Volume: "
    std::string suffix;  // Ex: "%"
    std::function<std::string()> valueGetter;  // Fonction pour récupérer la valeur
} DynamicText_t;

// ============= PARTICLE SYSTEM =============

// Types de particules prédéfinis
enum class ParticleType {
    Explosion,   // Éclate dans toutes les directions
    Trail,       // Suit une entité (traînée)
    Smoke,       // Monte lentement, s'estompe
    Sparks,      // Petites, rapides, avec gravité
    Debris       // Gros morceaux qui tombent
};

// Particule individuelle
typedef struct Particle_s {
    bool active = false;           // Utilisée dans le pool
    float lifetime = 0.f;          // Temps restant
    float maxLifetime = 1.f;       // Durée totale
    float vx = 0.f, vy = 0.f;      // Vélocité
    float ax = 0.f, ay = 0.f;      // Accélération (gravité, vent)
    float x = 0.f, y = 0.f;        // Position
    float size = 4.f;              // Taille actuelle
    float startSize = 4.f;         // Taille initiale
    float endSize = 0.f;           // Taille finale
    float rotation = 0.f;          // Angle
    float rotationSpeed = 0.f;     // Vitesse de rotation
    sf::Color startColor = sf::Color::White;
    sf::Color endColor = sf::Color(255, 255, 255, 0);
} Particle_t;

// Émetteur de particules (attaché à une entité)
typedef struct ParticleEmitter_s {
    ParticleType type = ParticleType::Explosion;
    float spawnRate = 50.f;        // Particules par seconde
    float spawnAccumulator = 0.f;  // Accumulateur pour le spawn
    int maxParticles = 100;        // Max particules par émission
    int particlesPerBurst = 20;    // Particules par burst (pour Explosion)
    float emitterLifetime = -1.f;  // Durée de l'émetteur (-1 = infini)
    float particleLifetime = 1.f;  // Durée de vie des particules
    float speed = 200.f;           // Vitesse initiale des particules
    float spread = 360.f;          // Angle de dispersion (degrés)
    float direction = 0.f;         // Direction principale (degrés)
    float gravity = 0.f;           // Gravité (+ = vers le bas)
    float startSize = 4.f;
    float endSize = 0.f;
    sf::Color startColor = sf::Color::Yellow;
    sf::Color endColor = sf::Color(255, 100, 0, 0);
    bool active = true;
    bool burst = false;            // true = émet tout d'un coup, false = continu
} ParticleEmitter_t;
