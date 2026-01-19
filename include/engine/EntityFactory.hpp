/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Entity Factory
*/
#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include "engine/systems/RessourceManager.hpp"
#include "engine/Menu.hpp"
#include <cstring>
#include <string>

namespace Factory {

    // ============================================================
    // COLLIDER SIZE HELPERS - Ensure server/client consistency
    // ============================================================
    inline Collider_t createPlayerCollider() {
        return Collider_t{64.f, 64.f, true, 1, 50};
    }

    inline Collider_t createEnemyCollider() {
        return Collider_t{64.f, 64.f, true, 2, 30};
    }

    inline Collider_t createProjectileCollider(uint8_t team, int damage) {
        return Collider_t{16.f, 8.f, false, team, damage};
    }

    inline Collider_t createTileCollider() {
        // Block sprite rendered at 0.5 scale; shrink collider accordingly to keep visuals and physics aligned
        constexpr float baseW = 259.f;
        constexpr float baseH = 258.f;
        constexpr float tileScale = 0.5f;
        return Collider_t{baseW * tileScale, baseH * tileScale, true, 0, 0};
    }

    // Forward declare createSound to use in createProjectile
    inline PlaySound_t createSound(const std::string& soundId, float volume = 100.f, float pitch = 1.0f);

    // Crée un Drawable avec une seule frame (pas d'animation)
    inline Drawable_t createDrawable(const std::string& textureName, sf::IntRect rect, int layer, bool visible = true, float scale = 1.f, float rotation = 0.f)
    {
        Drawable_t d;
        d.textureName = textureName;
        d.frames.push_back(rect);  // Ajoute le rect comme première frame
        d.currentFrameIndex = 0;
        d.animationSpeed = 0.1f;
        d.frameTimer = 0.0f;
        d.loop = true;
        d.layer = layer;
        d.visible = visible;
        d.scale = scale;
        d.rotation = rotation;
        return d;
    }

    // Crée un Drawable avec plusieurs frames (animation)
    inline Drawable_t createAnimatedDrawable(const std::string& textureName, const std::vector<sf::IntRect>& frames, 
                                             float animationSpeed, bool loop, int layer, 
                                             bool visible = true, float scale = 1.f, float rotation = 0.f)
    {
        Drawable_t d;
        d.textureName = textureName;
        d.frames = frames;
        d.currentFrameIndex = 0;
        d.animationSpeed = animationSpeed;
        d.frameTimer = 0.0f;
        d.loop = loop;
        d.layer = layer;
        d.visible = visible;
        d.scale = scale;
        d.rotation = rotation;
        return d;
    }

    Entity createPlayer(ECS& ecs, float x, float y, uint8_t playerId, const std::string& textureName)
    {
        Entity player = ecs.createEntity();

        // On server side, ResourceManager might not be initialized, so use default rect
        sf::IntRect spriteRect{0, 0, 124, 64};
        try {
            ResourceManager& rm = ResourceManager::getInstance();
            spriteRect = rm.getSpriteRect(textureName);
        } catch (...) {
            // Server side - ResourceManager not initialized, use default
        }

        ecs.addComponents
            <Position_t, Velocity_t, PlayerController_t, Drawable_t, Collider_t, Health_t, SendUpdate_t>
            (player,
                Position_t{x, y},
                Velocity_t{0.f, 0.f},
                PlayerController_t{playerId, false},
                Factory::createDrawable(textureName, spriteRect, 10, true, 1.f, 0.f),
                createPlayerCollider(),
                Health_t{200, 200},
                SendUpdate_t{true});
        return player;
    }

    Entity createProjectile(ECS& ecs, float x, float y, float velocityX, float velocityY,
                            uint8_t team, int damage, const std::string& textureName = "bullet", int ownerId = -1,
                            const std::string& shootSound = "shoot.ogg")
    {
        Entity bullet = ecs.createEntity();

        // On server side, ResourceManager might not be initialized, so use default rect
        sf::IntRect spriteRect{0, 0, 16, 8};
        try {
            ResourceManager& rm = ResourceManager::getInstance();
            spriteRect = rm.getSpriteRect(textureName);
        } catch (...) {
            // Server side - ResourceManager not initialized, use default
        }

        ecs.addComponents
            <Position_t, Velocity_t, Drawable_t, Collider_t, Projectile_t, PlaySound_t>
            (bullet,
                Position_t{x, y},
                Velocity_t{velocityX, velocityY},
                Factory::createDrawable(textureName, spriteRect, 20, true, 0.1f, 0.f),
                createProjectileCollider(team, damage),
                Projectile_t{velocityX, damage, ownerId},
                createSound(shootSound, 80.f, 1.0f));
                // SendUpdate_t{true});
        return bullet;
    }

Entity createEnemy(ECS& ecs, float x, float y, MovementPattern_t::Type movementType) {
    Entity enemy = ecs.createEntity();

    // Configurer le mouvement
    MovementPattern_t movement;
    movement.type = movementType;

    switch (movementType) {
        case MovementPattern_t::Type::Linear:
            movement.speed = 200.f;
            break;
        case MovementPattern_t::Type::Sinus:
            movement.speed = 200.f;
            movement.amplitude = 50.f;
            movement.frequency = 1.f;
            break;
        case MovementPattern_t::Type::Zigzag:
            movement.speed = 200.f;
            movement.amplitude = 50.f;
            movement.frequency = 1.f;
            break;
        case MovementPattern_t::Type::Spiral:
            movement.speed = 150.f;
            movement.radius = 100.f;
            break;
        default:
            movement.speed = 200.f;
            break;
    }

    // On server side, ResourceManager might not be initialized, so use default rect
    sf::IntRect spriteRect{0, 0, 64, 64};
    try {
        ResourceManager& rm = ResourceManager::getInstance();
        spriteRect = rm.getSpriteRect("enemy");
    } catch (...) {
        // Server side - ResourceManager not initialized, use default
    }

    // Créer l'ennemi avec tous les composants nécessaires
    ecs.addComponents<Position_t, Velocity_t, Health_t, Collider_t, Drawable_t, Enemy_t, MovementPattern_t, SendUpdate_t>(
        enemy,
        Position_t{x, y},  // Position
        Velocity_t{0.f, 0.f},  // Vitesse initiale
        Health_t{100, 100},  // Santé (current, max)
        createEnemyCollider(),  // Collider (use helper function)
        Factory::createDrawable("enemy", spriteRect, 10, true, 1.f, 0.f),  // Drawable (use helper function)
        Enemy_t{},  // Enemy_t
        std::move(movement),  // MovementPattern_t
        SendUpdate_t{}  // SendUpdate_t
    );

    return enemy;
}

void playDeathSound(ECS& ecs, Entity entity, const std::string& soundId = "enemy_explosion.wav", float volume = 90.f)
{
    if (ecs.hasComponent<PlaySound_t>(entity)) {
        // Évite le double son
        return;
    }
    ecs.addComponent(entity, createSound(soundId, volume));
}

inline PlaySound_t createSound(const std::string& soundId, float volume, float pitch)
{
    PlaySound_t s;
    std::memset(s.soundId, 0, sizeof(s.soundId));
    std::strncpy(s.soundId, soundId.c_str(), sizeof(s.soundId) - 1);
    s.volume = volume;
    s.pitch = pitch;
    return s;
}

inline Text_t createText(
    const std::string& textStr,
    uint32_t fontSize = 48,
    sf::Color color = sf::Color::White,
    bool centered = true,
    const std::string& fontId = "default",
    bool visible = true
) {
    Text_t t;
    std::memset(t.text, 0, sizeof(t.text));
    std::strncpy(t.text, textStr.c_str(), sizeof(t.text) - 1);
    t.text[sizeof(t.text) - 1] = '\0';
    t.fontSize = fontSize;
    t.color = color;
    t.originalColor = color;  // Sauvegarde la couleur originale
    t.centered = centered;
    std::strncpy(t.fontId, fontId.c_str(), sizeof(t.fontId) - 1);
    t.visible = visible;
    return t;
}

Entity createMenuTitle(ECS& ecs, const Menu& menu)
{
    Entity titleEntity = ecs.createEntity();
    ecs.addComponents<Position_t, Text_t>(
        titleEntity,
        Position_t{menu.titleXPosition, menu.titleYPosition},
        createText(menu.title, menu.titleFontSize, menu.titleColor, menu.titleCentered, menu.titleFontId, true)
    );
    return titleEntity;
}

Entity createMenuItem(ECS& ecs, const MenuItem& item) {
    Entity menuItem = ecs.createEntity();
    ecs.addComponents<Position_t, Text_t, MenuItem_t, Highlight_t>(
        menuItem,
        Position_t{item.xPosition, item.yPosition},
        createText(item.text, item.fontSize, item.color, item.centered, item.fontId, item.visible),
        MenuItem_t{item.action, false, item.isSelectable},
        Highlight_t{item.selectedColor, item.scale}
    );
    return menuItem;
}

// Crée un menu complet à partir d'une structure Menu
void createMenu(ECS& ecs, const Menu& menu)
{
    // Crée le titre si défini
    if (!menu.title.empty()) {
        createMenuTitle(ecs, menu);
    }

    // Crée les éléments du menu
    for (const auto& item : menu.items) {
        createMenuItem(ecs, item);
    }

    // Joue la musique de fond si définie
    if (!menu.backgroundMusicId.empty()) {
        Entity musicEntity = ecs.createEntity();
        BackgroundMusic_t musicComp;
        std::strncpy(musicComp.musicId, menu.backgroundMusicId.c_str(), sizeof(musicComp.musicId) - 1);
        musicComp.musicId[sizeof(musicComp.musicId) - 1] = '\0';  // Assure le terminateur nul
        musicComp.looping = true;
        musicComp.volume = 40.f;
        ecs.addComponent(musicEntity, musicComp);
    }
}

Entity createDestructibleTile(ECS& ecs, float x, float y, int health, const std::string& textureName)
    {
        Entity tile = ecs.createEntity();
        
        // On server side, ResourceManager might not be initialized, so use default rect for 'block' sprite
        sf::IntRect spriteRect{0, 0, 259, 258};  // Default to 'block' sprite size from atlas
        try {
            ResourceManager& rm = ResourceManager::getInstance();
            spriteRect = rm.getSpriteRect(textureName);
        } catch (...) {
            // Server side - ResourceManager not initialized, use default
        }

        ecs.addComponents<Position_t, Drawable_t, Collider_t, Health_t, DestructibleTile_t, SendUpdate_t>
            (tile,
                Position_t{x, y},
                Factory::createDrawable(textureName, spriteRect, 5, true, 1.f, 0.f),
                createTileCollider(),
                Health_t{health, health},
                DestructibleTile_t{true},
                SendUpdate_t{true});
        return tile;
    }

// Supprime un menu existant
void clearMenu(ECS& ecs)
{
    auto menuItems = ecs.getEntitiesByComponents<Position_t, Text_t, MenuItem_t>();
    for (Entity e : menuItems) {
        ecs.killEntity(e);
    }
    auto titles = ecs.getEntitiesByComponents<Position_t, Text_t>();
    for (Entity e : titles) {
        // Vérifie que c'est bien un titre de menu (simplification)
        ecs.killEntity(e);
    }
    auto musics = ecs.getEntitiesByComponents<BackgroundMusic_t>();
    for (Entity e : musics) {
        ecs.killEntity(e);
    }
}

// ============= PLATFORMER ENTITIES =============

// Crée un personnage de plateforme avec physique complète
// NOTE: PAS de PlayerController_t car ça active l'InputSystem de R-Type qui écrase la gravité !
inline Entity createPlatformerPlayer(ECS& ecs, float x, float y, const std::string& textureId = "player")
{
    Entity player = ecs.createEntity();
    // IMPORTANT: On utilise PlatformerController_t au lieu de PlayerController_t
    // pour éviter que l'InputSystem écrase les vélocités (MoveUp/MoveDown = vol libre)
    ecs.addComponents<Position_t, Velocity_t, RigidBody_t, Jumper_t, BoxCollider_t, Drawable_t>(
        player,
        Position_t{x, y},
        Velocity_t{0.f, 0.f},
        RigidBody_t{1.f, 1.f, 0.2f, 0.f, true, false, false, 5.f},  // masse, gravité, drag (↑ pour moins glisser), bounce, useGravity, kinematic, grounded, groundCheck
        Jumper_t{650.f, 2, 0, true, 0.15f, 0.f, 0.1f, 0.f},  // jumpForce (↑ pour sauter plus haut), maxJumps, currentJumps, canJump, coyoteTime, coyoteCounter, jumpBuffer, bufferCounter
        BoxCollider_t{48.f, 64.f, 8.f, 0.f, false, 0, 0xFFFFFFFF},  // width, height, offsetX, offsetY, trigger, layer, mask
        Factory::createDrawable(textureId, {0, 0, 64, 64}, 10, true, 1.f, 0.f)
        // PAS de PlayerController_t ici ! Le platformer gère ses inputs manuellement.
    );
    return player;
}

// Crée une plateforme statique
inline Entity createPlatform(ECS& ecs, float x, float y, float width, float height, 
                             bool oneWay = false, const std::string& textureId = "platform")
{
    Entity platform = ecs.createEntity();
    ecs.addComponents<Position_t, Platform_t, BoxCollider_t, Drawable_t, RigidBody_t>(
        platform,
        Position_t{x, y},
        Platform_t{oneWay, false, 1.f, {0.f, 0.f}},
        BoxCollider_t{width, height, 0.f, 0.f, false, 1, 0xFFFFFFFF},
        Factory::createDrawable(textureId, {0, 0, static_cast<int>(width), static_cast<int>(height)}, 5, true, 1.f, 0.f),
        RigidBody_t{100.f, 0.f, 0.f, 0.f, false, true, false, 0.f}  // Kinematic = ne bouge pas
    );
    return platform;
}

// Crée une plateforme mouvante
inline Entity createMovingPlatform(ECS& ecs, float x, float y, float width, float height,
                                   float velX, float velY, const std::string& textureId = "platform")
{
    Entity platform = ecs.createEntity();
    ecs.addComponents<Position_t, Velocity_t, Platform_t, BoxCollider_t, Drawable_t, RigidBody_t>(
        platform,
        Position_t{x, y},
        Velocity_t{velX, velY},
        Platform_t{false, false, 1.f, {velX, velY}},
        BoxCollider_t{width, height, 0.f, 0.f, false, 1, 0xFFFFFFFF},
        Factory::createDrawable(textureId, {0, 0, static_cast<int>(width), static_cast<int>(height)}, 5, true, 1.f, 0.f),
        RigidBody_t{100.f, 0.f, 0.f, 0.f, false, true, false, 0.f}
    );
    return platform;
}

// Crée un mur/obstacle
inline Entity createWall(ECS& ecs, float x, float y, float width, float height, 
                         const std::string& textureId = "wall")
{
    Entity wall = ecs.createEntity();
    ecs.addComponents<Position_t, BoxCollider_t, Drawable_t, RigidBody_t>(
        wall,
        Position_t{x, y},
        BoxCollider_t{width, height, 0.f, 0.f, false, 1, 0xFFFFFFFF},
        Factory::createDrawable(textureId, {0, 0, static_cast<int>(width), static_cast<int>(height)}, 5, true, 1.f, 0.f),
        RigidBody_t{1000.f, 0.f, 0.f, 0.f, false, true, false, 0.f}
    );
    return wall;
}

// Crée une échelle
inline Entity createLadder(ECS& ecs, float x, float y, float width, float height, 
                           float climbSpeed = 150.f, const std::string& textureId = "ladder")
{
    Entity ladder = ecs.createEntity();
    ecs.addComponents<Position_t, Ladder_t, BoxCollider_t, Drawable_t>(
        ladder,
        Position_t{x, y},
        Ladder_t{climbSpeed},
        BoxCollider_t{width, height, 0.f, 0.f, true, 2, 0xFFFFFFFF},  // Trigger
        Factory::createDrawable(textureId, {0, 0, static_cast<int>(width), static_cast<int>(height)}, 4, true, 1.f, 0.f)
    );
    return ladder;
}

// Crée une zone de téléportation
inline Entity createTeleporter(ECS& ecs, float x, float y, float width, float height,
                               sf::Vector2f targetPos, const std::string& textureId = "teleporter")
{
    Entity teleporter = ecs.createEntity();
    
    ecs.addComponents<Position_t, BoxCollider_t, Drawable_t>(
        teleporter,
        Position_t{x, y},
        BoxCollider_t{width, height, 0.f, 0.f, true, 3, 0xFFFFFFFF},
        Factory::createDrawable(textureId, {0, 0, static_cast<int>(width), static_cast<int>(height)}, 3, true, 1.f, 0.f)
    );
    
    // Ajoute le TriggerZone après car il contient une lambda
    TriggerZone_t trigger;
    trigger.type = TriggerZone_t::Type::Teleporter;
    trigger.teleportPos = targetPos;
    trigger.triggered = false;
    trigger.resetOnExit = true;
    trigger.onTrigger = [targetPos, &ecs](Entity entity) {
        auto* pos = ecs.getComponent<Position_t>(entity);
        if (pos) {
            pos->x = targetPos.x;
            pos->y = targetPos.y;
        }
    };
    ecs.addComponent(teleporter, trigger);
    
    return teleporter;
}

// Crée une zone de mort (kill zone)
inline Entity createDeathZone(ECS& ecs, float x, float y, float width, float height)
{
    Entity deathZone = ecs.createEntity();
    
    ecs.addComponents<Position_t, BoxCollider_t>(
        deathZone,
        Position_t{x, y},
        BoxCollider_t{width, height, 0.f, 0.f, true, 4, 0xFFFFFFFF}
    );
    
    TriggerZone_t trigger;
    trigger.type = TriggerZone_t::Type::Death;
    trigger.triggered = false;
    trigger.resetOnExit = false;
    trigger.onTrigger = [&ecs](Entity entity) {
        // Tue l'entité qui entre dans la zone
        auto* health = ecs.getComponent<Health_t>(entity);
        if (health) {
            health->current = 0;
        }
    };
    ecs.addComponent(deathZone, trigger);
    
    return deathZone;
}

// Crée un checkpoint
inline Entity createCheckpoint(ECS& ecs, float x, float y, const std::string& textureId = "checkpoint")
{
    Entity checkpoint = ecs.createEntity();
    
    ecs.addComponents<Position_t, BoxCollider_t, Drawable_t>(
        checkpoint,
        Position_t{x, y},
        BoxCollider_t{64.f, 64.f, 0.f, 0.f, true, 5, 0xFFFFFFFF},
        Factory::createDrawable(textureId, {0, 0, 64, 64}, 3, true, 1.f, 0.f)
    );
    
    TriggerZone_t trigger;
    trigger.type = TriggerZone_t::Type::Checkpoint;
    trigger.triggered = false;
    trigger.resetOnExit = false;
    trigger.teleportPos = sf::Vector2f(x, y);
    ecs.addComponent(checkpoint, trigger);
    
    return checkpoint;
}

// Crée un objet ramassable (collectible)
inline Entity createCollectible(ECS& ecs, float x, float y, const std::string& textureId = "coin")
{
    Entity collectible = ecs.createEntity();
    
    ecs.addComponents<Position_t, BoxCollider_t, Drawable_t>(
        collectible,
        Position_t{x, y},
        BoxCollider_t{32.f, 32.f, 0.f, 0.f, true, 6, 0xFFFFFFFF},
        Factory::createDrawable(textureId, {0, 0, 32, 32}, 8, true, 1.f, 0.f)
    );
    
    TriggerZone_t trigger;
    trigger.type = TriggerZone_t::Type::Custom;
    trigger.triggered = false;
    trigger.resetOnExit = false;
    trigger.onTrigger = [&ecs, collectible](Entity entity) {
        // Détruit le collectible quand il est ramassé
        ecs.killEntity(collectible);
        // Ici, tu peux ajouter du code pour incrémenter un score, jouer un son, etc.
    };
    ecs.addComponent(collectible, trigger);
    
    return collectible;
}

} // namespace Factory
