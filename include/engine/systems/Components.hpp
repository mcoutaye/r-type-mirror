/*
** EPITECH PROJECT, 2026
** R-type-mirror
** File description:
** Components hpp
*/

#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "engine/Core/Menu.hpp"
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

#define SHOOT_DELAY 0.5f

// Forward declare Entity type
using Entity = std::uint32_t;

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
    MenuSelect,
    ToggleDebug
};

/**
 * @namespace Components
 * @brief Espace dédié pour tous les composants ECS.
 * 
 * Ce namespace regroupe des structs de données pures (POD) sans logique ni héritage.
 * Ils sont utilisés par l'ECS pour attacher des propriétés aux entités.
 * Pas de relations directes entre eux, mais ils partagent un rôle commun dans l'architecture.
 * 
 * @see ECS pour l'intégration globale.
 */
namespace Components {

/**
 * @defgroup CompGroup Groupe des Composants ECS
 * @brief Sous-espace visuel pour les structs composants.
 * 
 * Tous ces structs sont des données pures, sans héritage.
 * Groupe pour visualiser ensemble dans les graphs Doxygen.
 * Pas de liens directs, mais rôle commun dans ECS.
 */
@{

    /**
     * @brief Should send updates to clients or not.
     * @struct SendUpdate_s
     */
    typedef struct SendUpdate_s {
        /**
         * @brief Flag indicating if the entity needs an update.
         */
        bool needsUpdate = false;
    } SendUpdate_t;

    /**
     * @brief Positions et mouvements (basiques)
     * @struct Position_s
     */
    typedef struct Position_s {
        /**
         * @brief X-coordinate of the position.
         */
        float x = 0.f;
        /**
         * @brief Y-coordinate of the position.
         */
        float y = 0.f;
    } Position_t;

    /**
     * @brief Velocity of an entity in 2D space.
     * @struct Velocity_s
     */
    typedef struct Velocity_s {
        /**
         * @brief X-component of the velocity.
         */
        float x = 0.f;
        /**
         * @brief Y-component of the velocity.
         */
        float y = 0.f;
    } Velocity_t;

    /**
     * @brief Render component for drawable entities.
     * @struct Drawable_s
     */
    typedef struct Drawable_s {
        /**
         * @brief Name of the texture to use.
         */
        std::string textureName;      // "ship1", "bullet", etc.
        /**
         * @brief Array of rectangles for animation frames.
         */
        std::vector<sf::IntRect> frames;  // Array de rectangles pour l'animation
        /**
         * @brief Index of the current frame.
         */
        size_t currentFrameIndex = 0;     // Index de la frame actuelle
        /**
         * @brief Time between each frame (in seconds).
         */
        float animationSpeed = 0.1f;      // Temps entre chaque frame (en secondes)
        /**
         * @brief Internal timer for animation.
         */
        float frameTimer = 0.0f;          // Timer interne pour l'animation
        /**
         * @brief If the animation should loop.
         */
        bool loop = true;                 // Si l'animation doit boucler
        /**
         * @brief Rendering layer (0=background, 10=player, etc.).
         */
        int layer = 0;                    // 0=background, 10=player, 20=bullets, 50=particles
        /**
         * @brief Visibility flag.
         */
        bool visible = true;
        /**
         * @brief Scale factor.
         */
        float scale = 1.f;
        /**
         * @brief Rotation angle.
         */
        float rotation = 0.f;
    } Drawable_t;

    /**
     * @brief Input / Joueur controller.
     * @struct PlayerController_s
     */
    typedef struct PlayerController_s {
        /**
         * @brief Player ID (0=player1, 1=player2...).
         */
        uint8_t playerId = 0;       // 0=joueur1, 1=joueur2...
        /**
         * @brief Shooting state flag.
         */
        bool isShooting = false;
        /**
         * @brief Cooldown timer for shooting.
         */
        float shootCooldown = 0.f;
    } PlayerController_t;

    /**
     * @brief Collision AABB simple.
     * @struct Collider_s
     */
    typedef struct Collider_s {
        /**
         * @brief Width of the collider.
         */
        float width = 32.f;
        /**
         * @brief Height of the collider.
         */
        float height = 32.f;
        /**
         * @brief If the collider is solid (blocking).
         */
        bool solid = true;          // bloquant ou pas
        /**
         * @brief Team ID for collision filtering.
         */
        uint8_t team = 0;           // 0=neutre, 1=joueurs, 2=ennemis
        /**
         * @brief Damage on collision.
         */
        int damage = 0;             // Dégâts sur collision
    } Collider_t;

    /**
     * @brief Projectile component.
     * @struct Projectile_s
     */
    typedef struct Projectile_s {
        /**
         * @brief Damage inflicted by the projectile.
         */
        int damage = 50;            // Dégâts
        /**
         * @brief Owner entity ID (-1 if none).
         */
        int ownerId = -1;           // ID du tireur
        /**
         * @brief Lifetime in seconds.
         */
        float lifetime = 5.f;       // Temps de vie (auto-destruction)
    } Projectile_t;

    /**
     * @brief Health component.
     * @struct Health_s
     */
    typedef struct Health_s {
        /**
         * @brief Current health value.
         */
        int current = 100;
        /**
         * @brief Maximum health value.
         */
        int max = 100;
    } Health_t;

    /**
     * @brief Movement pattern for entities (e.g., enemies).
     * @struct MovementPattern_s
     */
    typedef struct MovementPattern_s {
        /**
         * @brief Type of movement pattern.
         */
        enum class Type { Linear, Sinus, Cosinus, Circle, Zigzag, Spiral };
        Type type = Type::Linear;
        /**
         * @brief Amplitude of the movement.
         */
        float amplitude = 100.f;
        /**
         * @brief Frequency of the movement.
         */
        float frequency = 2.f;
        /**
         * @brief Speed of the movement.
         */
        float speed = 1.f;
        /**
         * @brief Radius for circular/spiral movements.
         */
        float radius = 50.f;
    } MovementPattern_t;

    /**
     * @brief Wave data for spawning.
     * @struct WaveData_s
     */
    typedef struct WaveData_s {
        /**
         * @brief Number of entities to spawn.
         */
        int count = 5;
        /**
         * @brief Delay before spawning (in seconds).
         */
        float delay = 5.f;
        /**
         * @brief X position for spawning.
         */
        float x = 1920.f;
        /**
         * @brief Y position for spawning.
         */
        float y = 540.f;
        /**
         * @brief Movement pattern type.
         */
        MovementPattern_t::Type movementType = MovementPattern_t::Type::Linear;
    } WaveData_t;

    /**
     * @brief Star for starfield effect.
     * @struct Star_s
     */
    typedef struct Star_s {
        /**
         * @brief Speed of the star.
         */
        float speed = 50.f;
        /**
         * @brief Brightness (0-255).
         */
        uint8_t brightness = 255;
        /**
         * @brief Size of the star.
         */
        uint8_t size = 1;
    } Star_t;

    /**
     * @brief Text rendering component.
     * @struct Text_s
     */
    typedef struct Text_s {
        /**
         * @brief Text content.
         */
        std::string content;
        /**
         * @brief Font ID.
         */
        std::string font = "default";
        /**
         * @brief Character size.
         */
        uint32_t characterSize = 40;
        /**
         * @brief Text color.
         */
        sf::Color color = sf::Color::White;
        /**
         * @brief Original color (for highlights).
         */
        sf::Color originalColor = sf::Color::White;
        /**
         * @brief Visibility flag.
         */
        bool visible = true;
    } Text_t;

    /**
     * @brief Highlight effect for menu items.
     * @struct Highlight_s
     */
    typedef struct Highlight_s {
        /**
         * @brief Color when selected.
         */
        sf::Color selectedColor = sf::Color::Yellow;
        /**
         * @brief Scale when selected.
         */
        float selectedScale = 1.2f;
    } Highlight_t;

    /**
     * @brief Menu item component.
     * @struct MenuItem_s
     */
    typedef struct MenuItem_s {
        /**
         * @brief Associated menu action.
         */
        MenuAction action;
        /**
         * @brief If the item is currently selected.
         */
        bool isSelected = false;
    } MenuItem_t;

    /**
     * @brief Slider component for options.
     * @struct Slider_s
     */
    typedef struct Slider_s {
        /**
         * @brief Minimum value.
         */
        float minValue = 0.f;
        /**
         * @brief Maximum value.
         */
        float maxValue = 100.f;
        /**
         * @brief Current value.
         */
        float currentValue = 50.f;
        /**
         * @brief Step increment.
         */
        float step = 1.f;
        /**
         * @brief Linked setting (e.g., "music_volume").
         */
        std::string linkedSetting;
    } Slider_t;

    /**
     * @brief Dynamic text component.
     * @struct DynamicText_s
     */
    typedef struct DynamicText_s {
        /**
         * @brief Prefix text.
         */
        std::string prefix;
        /**
         * @brief Suffix text.
         */
        std::string suffix;
        /**
         * @brief Function to get dynamic value as string.
         */
        std::function<std::string()> valueGetter;
    } DynamicText_t;

    /**
     * @brief Keybind button for remapping.
     * @struct KeybindButton_s
     */
    typedef struct KeybindButton_s {
        /**
         * @brief Associated game action.
         */
        GameAction action;
        /**
         * @brief If waiting for key input.
         */
        bool waitingForKey = false;
    } KeybindButton_t;

    /**
     * @brief Play sound trigger.
     * @struct PlaySound_s
     */
    typedef struct PlaySound_s {
        /**
         * @brief Sound ID to play.
         */
        std::string soundId;
        /**
         * @brief Volume (0-100).
         */
        float volume = 100.f;
        /**
         * @brief Pitch modifier.
         */
        float pitch = 1.0f;
        /**
         * @brief If the sound should loop.
         */
        bool loop = false;
    } PlaySound_t;

    /**
     * @brief Background music component.
     * @struct BackgroundMusic_s
     */
    typedef struct BackgroundMusic_s {
        /**
         * @brief Music ID to play.
         */
        std::string musicId;
        /**
         * @brief Volume (0-100).
         */
        float volume = 100.f;
        /**
         * @brief If the music should loop.
         */
        bool looping = true;
    } BackgroundMusic_t;

    /**
     * @brief Rigid body for physics.
     * @struct RigidBody_s
     */
    typedef struct RigidBody_s {
        /**
         * @brief Mass (kg).
         */
        float mass = 1.f;
        /**
         * @brief If kinematic (not affected by physics).
         */
        bool isKinematic = false;
        /**
         * @brief If affected by gravity.
         */
        bool useGravity = true;
        /**
         * @brief Air drag coefficient.
         */
        float drag = 0.1f;
        /**
         * @brief Ground friction coefficient.
         */
        float friction = 0.5f;
        /**
         * @brief Bounciness (0-1).
         */
        float bounciness = 0.f;
        /**
         * @brief If currently grounded.
         */
        bool isGrounded = false;
        /**
         * @brief Accumulated force.
         */
        sf::Vector2f accumulatedForce = {0.f, 0.f};
    } RigidBody_t;

    /**
     * @brief Jump component.
     * @struct Jump_s
     */
    typedef struct Jump_s {
        /**
         * @brief Jump force.
         */
        float jumpForce = 500.f;
        /**
         * @brief Coyote time (grace period after leaving ground).
         */
        float coyoteTime = 0.1f;
        /**
         * @brief Coyote timer.
         */
        float coyoteTimer = 0.f;
        /**
         * @brief Jump buffer time (input grace period).
         */
        float jumpBuffer = 0.1f;
        /**
         * @brief Jump buffer timer.
         */
        float jumpBufferTimer = 0.f;
        /**
         * @brief Maximum air jumps.
         */
        int maxAirJumps = 0;
        /**
         * @brief Remaining air jumps.
         */
        int airJumpsLeft = 0;
    } Jump_t;

    /**
     * @brief Particle emitter.
     * @struct ParticleEmitter_s
     */
    typedef struct ParticleEmitter_s {
        /**
         * @brief Type of particles.
         */
        enum class ParticleType { Explosion, Sparks, Smoke, Debris, Trail };
        ParticleType type = ParticleType::Explosion;
        /**
         * @brief Emission rate (particles/second).
         */
        float rate = 0.f;
        /**
         * @brief Timer for emission.
         */
        float timer = 0.f;
        /**
         * @brief If actively emitting.
         */
        bool emitting = false;
    } ParticleEmitter_t;

    /**
     * @brief Individual particle.
     * @struct Particle_s
     */
    typedef struct Particle_s {
        /**
         * @brief If active.
         */
        bool active = false;
        /**
         * @brief Position X.
         */
        float x = 0.f;
        /**
         * @brief Position Y.
         */
        float y = 0.f;
        /**
         * @brief Velocity X.
         */
        float vx = 0.f;
        /**
         * @brief Velocity Y.
         */
        float vy = 0.f;
        /**
         * @brief Acceleration X.
         */
        float ax = 0.f;
        /**
         * @brief Acceleration Y.
         */
        float ay = 0.f;
        /**
         * @brief Remaining lifetime.
         */
        float lifetime = 0.f;
        /**
         * @brief Maximum lifetime.
         */
        float maxLifetime = 0.f;
        /**
         * @brief Start size.
         */
        float startSize = 5.f;
        /**
         * @brief End size.
         */
        float endSize = 0.f;
        /**
         * @brief Start color.
         */
        sf::Color startColor = sf::Color::White;
        /**
         * @brief End color.
         */
        sf::Color endColor = sf::Color::Transparent;
        /**
         * @brief Rotation angle.
         */
        float rotation = 0.f;
        /**
         * @brief Rotation speed.
         */
        float rotationSpeed = 0.f;
    } Particle_t;

    /**
     * @brief Camera component.
     * @struct Camera_s
     */
    typedef struct Camera_s {
        /**
         * @brief Entity to follow.
         */
        Entity target = static_cast<Entity>(-1);
        /**
         * @brief Follow smoothness (0-1).
         */
        float followSpeed = 0.1f;
        /**
         * @brief Current zoom level.
         */
        float zoom = 1.f;
        /**
         * @brief Target zoom level.
         */
        float targetZoom = 1.f;
        /**
         * @brief Zoom speed.
         */
        float zoomSpeed = 2.f;
        /**
         * @brief Shake intensity.
         */
        float shakeIntensity = 0.f;
        /**
         * @brief Shake duration.
         */
        float shakeDuration = 0.f;
        /**
         * @brief Shake timer.
         */
        float shakeTimer = 0.f;
        /**
         * @brief If using bounds.
         */
        bool useBounds = false;
        /**
         * @brief Minimum X bound.
         */
        float minX = 0.f;
        /**
         * @brief Minimum Y bound.
         */
        float minY = 0.f;
        /**
         * @brief Maximum X bound.
         */
        float maxX = 0.f;
        /**
         * @brief Maximum Y bound.
         */
        float maxY = 0.f;
    } Camera_t;

    /**
     * @brief Obstacle component.
     * @struct Obstacle_s
     */
    typedef struct Obstacle_s {
        /**
         * @brief If destructible.
         */
        bool destructible = false;
    } Obstacle_t;

    /**
     * @brief Platform component.
     * @struct Platform_s
     */
    typedef struct Platform_s {
        /**
         * @brief If one-way (can jump through from below).
         */
        bool oneWay = false;
        /**
         * @brief Friction of the surface (0=slippery, 1=normal).
         */
        float friction = 1.f;         // Friction de la surface (0 = glissant, 1 = normal)
        /**
         * @brief Velocity of the platform (for moving platforms).
         */
        sf::Vector2f velocity = {0.f, 0.f};  // Vitesse de la plateforme (pour plateformes mouvantes)
    } Platform_t;

    /**
     * @brief Collision avancée pour les plateformes.
     * @struct BoxCollider_s
     */
    typedef struct BoxCollider_s {
        /**
         * @brief Width of the collider.
         */
        float width = 64.f;
        /**
         * @brief Height of the collider.
         */
        float height = 64.f;
        /**
         * @brief X offset from position.
         */
        float offsetX = 0.f;          // Décalage du collider par rapport à la position
        /**
         * @brief Y offset from position.
         */
        float offsetY = 0.f;
        /**
         * @brief If it's a trigger (detects but doesn't block).
         */
        bool isTrigger = false;       // Si true, détecte les collisions mais ne bloque pas
        /**
         * @brief Collision layer.
         */
        uint8_t layer = 0;            // Layer de collision (0 = default)
        /**
         * @brief Mask of layers to collide with.
         */
        uint32_t collisionMask = 0xFFFFFFFF;  // Masque des layers avec lesquels il collisionne
    } BoxCollider_t;

    /**
     * @brief Informations sur une collision détectée.
     * @struct CollisionInfo_s
     */
    typedef struct CollisionInfo_s {
        /**
         * @brief The other entity in the collision.
         */
        Entity other;                 // L'autre entité dans la collision
        /**
         * @brief Collision normal vector.
         */
        sf::Vector2f normal;          // Vecteur normal de la collision
        /**
         * @brief Penetration depth.
         */
        float penetration;            // Profondeur de pénétration
        /**
         * @brief Flag if ground collision.
         */
        bool isGroundCollision;       // Est-ce une collision avec le sol
    } CollisionInfo_t;

    /**
     * @brief Échelle/escalier.
     * @struct Ladder_s
     */
    typedef struct Ladder_s {
        /**
         * @brief Climb speed.
         */
        float climbSpeed = 150.f;     // Vitesse de montée/descente
    } Ladder_t;

    /**
     * @brief Zone de trigger (pour téléporteurs, checkpoints, etc.).
     * @struct TriggerZone_s
     */
    typedef struct TriggerZone_s {
        /**
         * @brief Enumeration of trigger types.
         */
        enum class Type { Teleporter, Checkpoint, Death, Win, Custom };
        /**
         * @brief Type of trigger.
         */
        Type type = Type::Custom;
        /**
         * @brief Target scene for teleporters.
         */
        std::string targetScene;      // Pour les téléporteurs
        /**
         * @brief Teleport position.
         */
        sf::Vector2f teleportPos;     // Position de téléportation
        /**
         * @brief Custom callback on trigger.
         */
        std::function<void(Entity)> onTrigger;  // Callback custom
        /**
         * @brief Flag if already triggered.
         */
        bool triggered = false;       // Déjà déclenché
        /**
         * @brief Flag to reset on exit.
         */
        bool resetOnExit = true;      // Se réinitialise quand on sort
    } TriggerZone_t;

    /**
     * @brief Composant pour marquer une entité comme ennemi.
     * @struct Enemy_s
     */
    typedef struct Enemy_s {
        /**
         * @brief Cooldown between shots.
         */
        float shootCooldown = 0.f;  // Délai entre les tirs
        /**
         * @brief Shooting range.
         */
        float shootRange = 600.f;   // Portée de tir
        /**
         * @brief Flag if can shoot.
         */
        bool canShoot = true;       // Type d'ennemi (ex: "basic", "boss")
    } Enemy_t;

@}
} // namespace Components