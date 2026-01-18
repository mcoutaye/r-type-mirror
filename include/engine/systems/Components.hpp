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
    MenuSelect
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
     * @struct SendUpdate
     */
    struct SendUpdate {
        /**
         * @brief Flag indicating if the entity needs an update.
         */
        bool needsUpdate = false;
    };

    /**
     * @brief Represents the position of an entity in 2D space.
     * @struct Position
     */
    struct Position {
        /**
         * @brief X-coordinate of the position.
         */
        float x = 0.f;

        /**
         * @brief Y-coordinate of the position.
         */
        float y = 0.f;
    };

    /**
     * @struct Velocity
     * @brief Represents the velocity of an entity in 2D space.
     */
    struct Velocity {
        /**
         * @brief X-component of the velocity.
         */
        float x = 0.f;

        /**
         * @brief Y-component of the velocity.
         */
        float y = 0.f;
    };

    /**
     * @struct Drawable
     * @brief Component for rendering entities.
     */
    struct Drawable {
        /**
         * @brief ID of the texture to use.
         */
        std::string textureId; // "ship.png", "bullet.png"

        /**
         * @brief Rectangle defining the frame in the texture.
         */
        sf::IntRect rect = {0, 0, 64, 64}; // frame actuelle

        /**
         * @brief Rendering layer (lower values drawn first).
         */
        int layer = 0; // 0=background, 10=player, 20=bullets, 50=particles

        /**
         * @brief Visibility flag.
         */
        bool visible = true;

        /**
         * @brief Scale factor for rendering.
         */
        float scale = 1.f;

        /**
         * @brief Rotation angle in degrees.
         */
        float rotation = 0.f;
    };

    /**
     * @struct PlayerController
     * @brief Component for player input control.
     */
    struct PlayerController {
        /**
         * @brief Unique player ID (0 for player 1, etc.).
         */
        uint8_t playerId = 0; // 0=joueur1, 1=joueur2...

        /**
         * @brief Flag indicating if the player is shooting.
         */
        bool isShooting = false;

        /**
         * @brief Cooldown time before next shot.
         */
        float shootCooldown = 0.f;
    };

    /**
     * @struct Collider
     * @brief Component for simple AABB collision detection.
     */
    struct Collider {
        /**
         * @brief Width of the collider box.
         */
        float width = 32.f;

        /**
         * @brief Height of the collider box.
         */
        float height = 32.f;

        /**
         * @brief Flag indicating if the collider is solid (blocking).
         */
        bool solid = true; // bloquant ou pas

        /**
         * @brief Team ID for collision filtering (0=neutral, 1=player, 2=enemy).
         */
        uint8_t team = 0; // 0=neutre, 1=player, 2=enemy

        /**
         * @brief Damage value on collision.
         */
        int damage = 1;
    };

    /**
     * @struct Health
     * @brief Component for tracking health points.
     */
    struct Health {
        /**
         * @brief Maximum health value.
         */
        int max = 100;

        /**
         * @brief Current health value.
         */
        int current = 100;

        /**
         * @brief ID of the last attacker.
         */
        int lastAttackerId = -1;
    };

    /**
     * @struct WaveSpawner
     * @brief Component for spawning waves of enemies.
     */
    struct WaveSpawner {
        /**
         * @brief Time until the next spawn.
         */
        float nextSpawnTime = 2.f; // temps avant prochain spawn

        /**
         * @brief Current wave number.
         */
        int currentWave = 0;
    };

    /**
     * @struct MovementPattern
     * @brief Defines movement behavior for entities.
     */
    struct MovementPattern {
        /**
         * @brief Enumeration of movement types.
         */
        enum class Type { Linear, Sinus, Cosinus, Circle, Zigzag, Spiral };

        /**
         * @brief The type of movement.
         */
        Type type;

        /**
         * @brief Amplitude for oscillatory movements.
         */
        float amplitude; // Pour les mouvements ondulatoires

        /**
         * @brief Frequency for oscillatory movements.
         */
        float frequency; // Pour les mouvements ondulatoires

        /**
         * @brief Radius for circular/spiral movements.
         */
        float radius; // Pour le cercle/spirale

        /**
         * @brief Speed for zigzag/spiral movements.
         */
        float speed; // Pour le zigzag/spirale
    };

    /**
     * @struct WaveData
     * @brief Data for a specific wave.
     */
    struct WaveData {
        /**
         * @brief Delay between spawns in the wave.
         */
        float delay;

        /**
         * @brief Type of enemy to spawn.
         */
        std::string enemyType;

        /**
         * @brief Movement type for the wave.
         */
        MovementPattern::Type movementType;

        /**
         * @brief Number of enemies in the wave.
         */
        int count;

        /**
         * @brief X-position for spawning.
         */
        float x;

        /**
         * @brief Y-position for spawning.
         */
        float y;
    };

    /**
     * @struct Projectile
     * @brief Component for projectile entities.
     */
    struct Projectile {
        /**
         * @brief Speed of the projectile.
         */
        float speed; // Vitesse du projectile

        /**
         * @brief Damage inflicted by the projectile.
         */
        int damage; // Dégâts infligés

        /**
         * @brief ID of the owner.
         */
        int ownerId = -1;
    };

    /**
     * @struct Shootable
     * @brief Component for entities that can shoot.
     */
    struct Shootable {
        /**
         * @brief Remaining cooldown time before next shot.
         */
        float cooldown; // Temps restant avant le prochain tir

        /**
         * @brief Delay between shots.
         */
        float shootDelay; // Délai entre deux tirs

        /**
         * @brief Speed of the missiles/projectiles.
         */
        float missileSpeed; // Vitesse des projectiles

        /**
         * @brief Damage inflicted by shots.
         */
        int damage; // Dégâts infligés

        /**
         * @brief Team ID of the shooter.
         */
        uint8_t team; // Équipe du tireur (pour éviter les tirs alliés)

        /**
         * @brief Texture ID for the projectile.
         */
        std::string textureId; // Texture du projectile

        /**
         * @brief X-offset for spawning the projectile.
         */
        float offsetX; // Offset X pour le spawn du projectile

        /**
         * @brief Y-offset for spawning the projectile.
         */
        float offsetY; // Offset Y pour le spawn du projectile

        /**
         * @brief Flag indicating if the entity is shooting.
         */
        bool isShooting; // Nouveau champ pour indiquer si le joueur tire

        /**
         * @brief Flag for triple shot ability.
         */
        bool tripleShot;
    };

    /**
     * @struct PowerUp
     * @brief Component for power-up effects.
     */
    struct PowerUp {
        /**
         * @brief Enumeration of power-up types.
         */
        enum class Type { TripleShot };

        /**
         * @brief The type of power-up.
         */
        Type type;

        /**
         * @brief Duration of the power-up effect in seconds.
         */
        float duration; // Durée de l'effet (en secondes)
    };

    /**
     * @struct Obstacle
     * @brief Component for obstacle entities.
     */
    struct Obstacle {
        /**
         * @brief Flag indicating if the obstacle blocks movement.
         */
        bool blocking = true; // Bloque le mouvement
    };

    /**
     * @struct DestructibleTile
     * @brief Component for destructible tiles.
     */
    struct DestructibleTile {
        /**
         * @brief Health points of the tile.
         */
        int health = 50; // Points de vie de la tuile
    };

    /**
     * @struct Star
     * @brief Component for background stars in a starfield.
     */
    struct Star {
        /**
         * @brief Scrolling speed of the star.
         */
        float speed = 100.f; // Vitesse de défilement

        /**
         * @brief Brightness level (0-255).
         */
        uint8_t brightness = 255; // Luminosité (pour effet de parallaxe)

        /**
         * @brief Size of the star.
         */
        uint8_t size = 2; // Taille de l'étoile
    };

    /**
     * @struct JustShot
     * @brief Component to indicate a recent shot.
     */
    struct JustShot {
        /**
         * @brief Flag indicating if the shot is active.
         */
        bool active = true;
    };

    /**
     * @struct PlaySound
     * @brief Trigger to play a sound effect once.
     */
    struct PlaySound {
        /**
         * @brief ID of the sound to play.
         */
        std::string soundId; // ex: "player_shoot.wav", "enemy_explosion.wav", "hit.wav"

        /**
         * @brief Volume level (0-100).
         */
        float volume = 100.f; // 0-100 (SFML utilise 0-100)

        /**
         * @brief Pitch variation.
         */
        float pitch = 1.0f; // variation de tonalité
    };

    /**
     * @struct BackgroundMusic
     * @brief Component for background music.
     */
    struct BackgroundMusic {
        /**
         * @brief ID of the music to play.
         */
        std::string musicId; // ex: "level1.ogg", "menu_theme.ogg"

        /**
         * @brief Flag for looping the music.
         */
        bool looping = true;

        /**
         * @brief Volume level.
         */
        float volume = 50.f;
    };

    /**
     * @struct Text
     * @brief Simple text for menu items (title, buttons...).
     */
    struct Text {
        /**
         * @brief The text content.
         */
        std::string text;

        /**
         * @brief ID of the font to use.
         */
        std::string fontId = "default";

        /**
         * @brief Font size.
         */
        uint32_t fontSize = 48;

        /**
         * @brief Text color.
         */
        sf::Color color;

        /**
         * @brief Original color for reset.
         */
        sf::Color originalColor; // Ajout du champ pour sauvegarder la couleur originale

        /**
         * @brief Flag for centering the text.
         */
        bool centered = true;

        /**
         * @brief Visibility flag.
         */
        bool visible = true;
    };

    /**
     * @struct MenuItem
     * @brief Indicates a selectable menu item.
     */
    struct MenuItem {
        /**
         * @brief Action associated with the item.
         */
        MenuAction action;

        /**
         * @brief Flag indicating if the item is selected.
         */
        bool isSelected = false;

        /**
         * @brief Flag indicating if the item is selectable.
         */
        bool isSelectable = true; // Peut être sélectionné par la navigation clavier/souris
    };

    /**
     * @struct Highlight
     * @brief Optional visual highlight (e.g., change color or scale when selected).
     */
    struct Highlight {
        /**
         * @brief Color when selected.
         */
        sf::Color selectedColor = sf::Color::Yellow;

        /**
         * @brief Scale when selected.
         */
        float selectedScale = 1.2f;
    };

    /**
     * @struct Slider
     * @brief Slider for controlling values (volume, sensitivity, etc.).
     */
    struct Slider {
        /**
         * @brief Minimum value.
         */
        float minValue = 0.0f;

        /**
         * @brief Maximum value.
         */
        float maxValue = 100.0f;

        /**
         * @brief Current value.
         */
        float currentValue = 50.0f;

        /**
         * @brief Step increment.
         */
        float step = 1.0f;

        /**
         * @brief Linked setting name.
         */
        std::string linkedSetting; // Ex: "music_volume", "sfx_volume"
    };

    /**
     * @brief Type of input for remapping.
     */
    enum class InputType {
        Keyboard,
        Joystick
    };

    /**
     * @struct KeybindButton
     * @brief Button for remapping a key or joystick button.
     */
    struct KeybindButton {
        /**
         * @brief Action to remap.
         */
        GameAction action; // L'action à remapper

        /**
         * @brief Type of input (keyboard or joystick).
         */
        InputType inputType = InputType::Keyboard; // Type d'input (clavier ou manette)

        /**
         * @brief Flag indicating if waiting for new input.
         */
        bool isWaitingForInput = false; // En attente d'une nouvelle touche/bouton

        /**
         * @brief Current keyboard key.
         */
        sf::Keyboard::Key currentKey = sf::Keyboard::Unknown;

        /**
         * @brief Current joystick button.
         */
        unsigned int currentJoystickButton = static_cast<unsigned int>(-1);
    };

    /**
     * @struct DynamicText
     * @brief Dynamic text that displays a value (for sliders or keybinds).
     */
    struct DynamicText {
        /**
         * @brief Prefix text.
         */
        std::string prefix; // Ex: "Volume: "

        /**
         * @brief Suffix text.
         */
        std::string suffix; // Ex: "%"

        /**
         * @brief Function to get the value.
         */
        std::function<std::string()> valueGetter; // Fonction pour récupérer la valeur
    };

    // ============= PARTICLE SYSTEM =============

    /**
     * @brief Predefined particle types.
     */
    enum class ParticleType {
        Explosion,   // Éclate dans toutes les directions
        Trail,       // Suit une entité (traînée)
        Smoke,       // Monte lentement, s'estompe
        Sparks,      // Petites, rapides, avec gravité
        Debris       // Gros morceaux qui tombent
    };

    /**
     * @struct Particle
     * @brief Individual particle.
     */
    struct Particle {
        /**
         * @brief Flag indicating if the particle is active.
         */
        bool active = false; // Utilisée dans le pool

        /**
         * @brief Remaining lifetime.
         */
        float lifetime = 0.f; // Temps restant

        /**
         * @brief Total lifetime.
         */
        float maxLifetime = 1.f; // Durée totale

        /**
         * @brief X-velocity.
         */
        float vx = 0.f;

        /**
         * @brief Y-velocity.
         */
        float vy = 0.f; // Vélocité

        /**
         * @brief X-acceleration.
         */
        float ax = 0.f;

        /**
         * @brief Y-acceleration.
         */
        float ay = 0.f; // Accélération (gravité, vent)

        /**
         * @brief X-position.
         */
        float x = 0.f;

        /**
         * @brief Y-position.
         */
        float y = 0.f; // Position

        /**
         * @brief Current size.
         */
        float size = 4.f; // Taille actuelle

        /**
         * @brief Starting size.
         */
        float startSize = 4.f; // Taille initiale

        /**
         * @brief Ending size.
         */
        float endSize = 0.f; // Taille finale

        /**
         * @brief Rotation angle.
         */
        float rotation = 0.f; // Angle

        /**
         * @brief Rotation speed.
         */
        float rotationSpeed = 0.f; // Vitesse de rotation

        /**
         * @brief Starting color.
         */
        sf::Color startColor = sf::Color::White;

        /**
         * @brief Ending color.
         */
        sf::Color endColor = sf::Color(255, 255, 255, 0);
    };

    /**
     * @struct ParticleEmitter
     * @brief Particle emitter attached to an entity.
     */
    struct ParticleEmitter {
        /**
         * @brief Type of particles.
         */
        ParticleType type = ParticleType::Explosion;

        /**
         * @brief Spawn rate (particles per second).
         */
        float spawnRate = 50.f; // Particules par seconde

        /**
         * @brief Accumulator for spawning.
         */
        float spawnAccumulator = 0.f; // Accumulateur pour le spawn

        /**
         * @brief Maximum particles per emission.
         */
        int maxParticles = 100; // Max particules par émission

        /**
         * @brief Particles per burst.
         */
        int particlesPerBurst = 20; // Particules par burst (pour Explosion)

        /**
         * @brief Emitter lifetime (-1 = infinite).
         */
        float emitterLifetime = -1.f; // Durée de l'émetteur (-1 = infini)

        /**
         * @brief Particle lifetime.
         */
        float particleLifetime = 1.f; // Durée de vie des particules

        /**
         * @brief Initial speed of particles.
         */
        float speed = 200.f; // Vitesse initiale des particules

        /**
         * @brief Spread angle (degrees).
         */
        float spread = 360.f; // Angle de dispersion (degrés)

        /**
         * @brief Main direction (degrees).
         */
        float direction = 0.f; // Direction principale (degrés)

        /**
         * @brief Gravity (+ = down).
         */
        float gravity = 0.f; // Gravité (+ = vers le bas)

        /**
         * @brief Starting size.
         */
        float startSize = 4.f;

        /**
         * @brief Ending size.
         */
        float endSize = 0.f;

        /**
         * @brief Starting color.
         */
        sf::Color startColor = sf::Color::Yellow;

        /**
         * @brief Ending color.
         */
        sf::Color endColor = sf::Color(255, 100, 0, 0);

        /**
         * @brief Flag indicating if active.
         */
        bool active = true;

        /**
         * @brief Flag for burst mode.
         */
        bool burst = false; // true = émet tout d'un coup, false = continu
    };

    // ============= CAMERA SYSTEM =============

    /**
     * @struct Camera
     * @brief 2D camera with smooth follow, zoom, and shake.
     */
    struct Camera {
        /**
         * @brief Current X position.
         */
        float x = 960.f; // Position actuelle X

        /**
         * @brief Current Y position.
         */
        float y = 540.f; // Position actuelle Y

        /**
         * @brief Target X position.
         */
        float targetX = 960.f; // Position cible X

        /**
         * @brief Target Y position.
         */
        float targetY = 540.f; // Position cible Y

        /**
         * @brief Smooth follow speed.
         */
        float smoothSpeed = 5.f; // Vitesse de suivi (plus élevé = plus réactif)

        /**
         * @brief Current zoom.
         */
        float zoom = 1.f; // Zoom actuel

        /**
         * @brief Target zoom.
         */
        float targetZoom = 1.f; // Zoom cible

        /**
         * @brief Zoom transition speed.
         */
        float zoomSpeed = 3.f; // Vitesse de transition du zoom

        /**
         * @brief Shake intensity.
         */
        float shakeIntensity = 0.f; // Intensité du shake

        /**
         * @brief Remaining shake duration.
         */
        float shakeDuration = 0.f; // Durée restante du shake

        /**
         * @brief Shake X offset.
         */
        float shakeOffsetX = 0.f; // Offset X du shake

        /**
         * @brief Shake Y offset.
         */
        float shakeOffsetY = 0.f; // Offset Y du shake

        /**
         * @brief View width.
         */
        float viewWidth = 1920.f; // Largeur de la vue

        /**
         * @brief View height.
         */
        float viewHeight = 1080.f; // Hauteur de la vue

        /**
         * @brief Flag to use world bounds.
         */
        bool useBounds = false; // Utilise des limites de monde

        /**
         * @brief Minimum X bound.
         */
        float minX = 0.f; // Limite gauche

        /**
         * @brief Minimum Y bound.
         */
        float minY = 0.f; // Limite haut

        /**
         * @brief Maximum X bound.
         */
        float maxX = 1920.f; // Limite droite

        /**
         * @brief Maximum Y bound.
         */
        float maxY = 1080.f; // Limite bas
    };

    /**
     * @struct CameraTarget
     * @brief Tag to mark an entity as camera target.
     */
    struct CameraTarget {
        /**
         * @brief X offset relative to entity position.
         */
        float offsetX = 0.f; // Décalage X par rapport à la position de l'entité

        /**
         * @brief Y offset relative to entity position.
         */
        float offsetY = 0.f; // Décalage Y par rapport à la position de l'entité
    };

    // ============= PHYSICS SYSTEM =============

    /**
     * @struct RigidBody
     * @brief Rigidbody for physics (gravity, forces, etc.).
     */
    struct RigidBody {
        /**
         * @brief Mass of the object.
         */
        float mass = 1.f; // Masse de l'objet

        /**
         * @brief Gravity multiplier.
         */
        float gravityScale = 1.f; // Multiplicateur de gravité (0 = pas de gravité)

        /**
         * @brief Air resistance (0-1).
         */
        float drag = 0.f; // Résistance de l'air (0-1, 0 = pas de résistance)

        /**
         * @brief Bounciness on collisions (0-1).
         */
        float bounciness = 0.f; // Rebond lors des collisions (0-1)

        /**
         * @brief Flag to apply gravity.
         */
        bool useGravity = true; // Applique la gravité ou non

        /**
         * @brief Flag if kinematic (not affected by forces but can affect others).
         */
        bool isKinematic = false; // Si true, pas affecté par les forces mais peut affecter les autres

        /**
         * @brief Flag if grounded.
         */
        bool isGrounded = false; // Est au sol (détecté par les collisions)

        /**
         * @brief Distance to check for ground.
         */
        float groundCheckDistance = 2.f; // Distance pour vérifier le sol
    };

    /**
     * @struct Jumper
     * @brief Component for entities that can jump.
     */
    struct Jumper {
        /**
         * @brief Jump force.
         */
        float jumpForce = 500.f; // Force du saut

        /**
         * @brief Maximum jumps allowed.
         */
        int maxJumps = 1; // Nombre de sauts autorisés (1 = simple saut, 2 = double saut)

        /**
         * @brief Current jumps performed.
         */
        int currentJumps = 0; // Nombre de sauts effectués

        /**
         * @brief Flag if can jump now.
         */
        bool canJump = true; // Peut sauter maintenant

        /**
         * @brief Coyote time (time to jump after leaving platform).
         */
        float coyoteTime = 0.1f; // Temps pendant lequel on peut sauter après avoir quitté une plateforme

        /**
         * @brief Coyote time counter.
         */
        float coyoteCounter = 0.f; // Compteur pour le coyote time

        /**
         * @brief Jump buffer time (memorize jump input).
         */
        float jumpBufferTime = 0.1f; // Temps pendant lequel un input de saut est mémorisé

        /**
         * @brief Jump buffer counter.
         */
        float jumpBufferCounter = 0.f; // Compteur pour le jump buffer
    };

    /**
     * @struct Platform
     * @brief Platform (surface to walk on).
     */
    struct Platform {
        /**
         * @brief Flag for one-way platform.
         */
        bool oneWay = false; // Plateforme traversable par le bas

        /**
         * @brief Flag if can move through with down + jump.
         */
        bool canMoveThrough = false; // Peut être traversée avec bas + saut

        /**
         * @brief Surface friction (0 = slippery, 1 = normal).
         */
        float friction = 1.f; // Friction de la surface (0 = glissant, 1 = normal)

        /**
         * @brief Platform velocity (for moving platforms).
         */
        sf::Vector2f velocity = {0.f, 0.f}; // Vitesse de la plateforme (pour plateformes mouvantes)
    };

    /**
     * @struct BoxCollider
     * @brief Advanced collision for platforms.
     */
    struct BoxCollider {
        /**
         * @brief Width of the collider.
         */
        float width = 64.f;

        /**
         * @brief Height of the collider.
         */
        float height = 64.f;

        /**
         * @brief X offset relative to position.
         */
        float offsetX = 0.f; // Décalage du collider par rapport à la position

        /**
         * @brief Y offset relative to position.
         */
        float offsetY = 0.f;

        /**
         * @brief Flag if trigger (detects but doesn't block).
         */
        bool isTrigger = false; // Si true, détecte les collisions mais ne bloque pas

        /**
         * @brief Collision layer.
         */
        uint8_t layer = 0; // Layer de collision (0 = default)

        /**
         * @brief Mask of layers to collide with.
         */
        uint32_t collisionMask = 0xFFFFFFFF; // Masque des layers avec lesquels il collisionne
    };

    /**
     * @struct CollisionInfo
     * @brief Information about a detected collision.
     */
    struct CollisionInfo {
        /**
         * @brief The other entity in the collision.
         */
        Entity other; // L'autre entité dans la collision

        /**
         * @brief Collision normal vector.
         */
        sf::Vector2f normal; // Vecteur normal de la collision

        /**
         * @brief Penetration depth.
         */
        float penetration; // Profondeur de pénétration

        /**
         * @brief Flag if ground collision.
         */
        bool isGroundCollision; // Est-ce une collision avec le sol
    };

    /**
     * @struct Ladder
     * @brief Ladder/climbable area.
     */
    struct Ladder {
        /**
         * @brief Climb speed.
         */
        float climbSpeed = 150.f; // Vitesse de montée/descente
    };

    /**
     * @struct TriggerZone
     * @brief Trigger zone (for teleporters, checkpoints, etc.).
     */
    struct TriggerZone {
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
        std::string targetScene; // Pour les téléporteurs

        /**
         * @brief Teleport position.
         */
        sf::Vector2f teleportPos; // Position de téléportation

        /**
         * @brief Custom callback on trigger.
         */
        std::function<void(Entity)> onTrigger; // Callback custom

        /**
         * @brief Flag if already triggered.
         */
        bool triggered = false; // Déjà déclenché

        /**
         * @brief Flag to reset on exit.
         */
        bool resetOnExit = true; // Se réinitialise quand on sort
    };

    /**
     * @struct Enemy
     * @brief Component to mark an entity as enemy.
     */
    struct Enemy {
        /**
         * @brief Cooldown between shots.
         */
        float shootCooldown = 0.f; // Délai entre les tirs

        /**
         * @brief Shooting range.
         */
        float shootRange = 600.f; // Portée de tir

        /**
         * @brief Flag if can shoot.
         */
        bool canShoot = true; // Type d'ennemi (ex: "basic", "boss")
    };

@}
}