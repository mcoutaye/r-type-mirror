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
        
        // On server side, RessourceManager might not be initialized, so use default rect
        sf::IntRect spriteRect{0, 0, 124, 64};
        try {
            RessourceManager& rm = RessourceManager::getInstance();
            spriteRect = rm.getSpriteRect(textureName);
        } catch (...) {
            // Server side - RessourceManager not initialized, use default
        }

        ecs.addComponents
            <Position_t, Velocity_t, PlayerController_t, Drawable_t, Collider_t, Health_t, SendUpdate_t>
            (player,
                Position_t{x, y},
                Velocity_t{0.f, 0.f},
                PlayerController_t{playerId, false},
                createDrawable(textureName, spriteRect, 10, true, 1.f, 0.f),
                createPlayerCollider(),
                Health_t{200, 200},
                SendUpdate_t{true});
        return player;
    }

    Entity createProjectile(ECS& ecs, float x, float y, float velocityX, float velocityY,
                            uint8_t team, int damage, const std::string& textureName, int ownerId = -1)
    {
        Entity bullet = ecs.createEntity();

        // On server side, RessourceManager might not be initialized, so use default rect
        sf::IntRect spriteRect{0, 0, 16, 8};
        try {
            RessourceManager& rm = RessourceManager::getInstance();
            spriteRect = rm.getSpriteRect(textureName);
        } catch (...) {
            // Server side - RessourceManager not initialized, use default
        }

        ecs.addComponents
            <Position_t, Velocity_t, Drawable_t, Collider_t, Projectile_t>
            (bullet,
                Position_t{x, y},
                Velocity_t{velocityX, velocityY},
                createDrawable(textureName, spriteRect, 20, true, 0.1f, 0.f),
                createProjectileCollider(team, damage),
                Projectile_t{velocityX, damage, ownerId});
                // SendUpdate_t{true});
        return bullet;
    }

    Entity createEnemy(ECS& ecs, float x, float y, int health,
                       MovementPattern_t::Type pattern, const std::string& textureName)
    {
        Entity enemy = ecs.createEntity();
        
        // On server side, RessourceManager might not be initialized, so use default rect
        sf::IntRect spriteRect{0, 0, 64, 64};
        try {
            RessourceManager& rm = RessourceManager::getInstance();
            spriteRect = rm.getSpriteRect(textureName);
        } catch (...) {
            // Server side - RessourceManager not initialized, use default
        }

        ecs.addComponents<Position_t, Velocity_t, Drawable_t, Collider_t, Health_t, MovementPattern_t, SendUpdate_t>
            (enemy,
                Position_t{x, y},
                Velocity_t{-100.f, 0.f},
                createDrawable(textureName, spriteRect, 10, true, 1.f, 0.f),
                createEnemyCollider(),
                Health_t{health, health},
                MovementPattern_t{pattern, 50.f, 2.f, 0.f, 100.f},
                SendUpdate_t{true});
        return enemy;
    }

    Entity createDestructibleTile(ECS& ecs, float x, float y, int health, const std::string& textureName)
    {
        Entity tile = ecs.createEntity();
        
        // On server side, RessourceManager might not be initialized, so use default rect for 'block' sprite
        sf::IntRect spriteRect{0, 0, 259, 258};  // Default to 'block' sprite size from atlas
        try {
            RessourceManager& rm = RessourceManager::getInstance();
            spriteRect = rm.getSpriteRect(textureName);
        } catch (...) {
            // Server side - RessourceManager not initialized, use default
        }

        ecs.addComponents<Position_t, Drawable_t, Collider_t, Health_t, DestructibleTile_t, SendUpdate_t>
            (tile,
                Position_t{x, y},
                createDrawable(textureName, spriteRect, 5, true, 1.f, 0.f),
                createTileCollider(),
                Health_t{health, health},
                DestructibleTile_t{true},
                SendUpdate_t{true});
        return tile;
    }

}