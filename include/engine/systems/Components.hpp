    /*
    ** EPITECH PROJECT, 2025
    ** R-type
    ** File description:
    ** Components hpp
    */
    #pragma once

    #include <SFML/Graphics.hpp>
    #include <string>

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
    @}
    }