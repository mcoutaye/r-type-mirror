/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Entity Factory
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include <cstring>
#include <string>

namespace Factory {

    inline Drawable_t createDrawable(const std::string& textureId, sf::IntRect rect, int layer, bool visible = true, float scale = 1.f, float rotation = 0.f)
    {
        Drawable_t d;
        std::memset(d.textureId, 0, sizeof(d.textureId));
        std::strncpy(d.textureId, textureId.c_str(), sizeof(d.textureId) - 1);
        d.rect = rect;
        d.layer = layer;
        d.visible = visible;
        d.scale = scale;
        d.rotation = rotation;
        return d;
    }

    Entity createPlayer(ECS& ecs, float x, float y, uint8_t playerId, const std::string& textureId)
    {
        Entity player = ecs.createEntity();

        ecs.addComponents
            <Position_t, Velocity_t, PlayerController_t, Drawable_t, Collider_t, Health_t, SendUpdate_t>
            (player,
                Position_t{x, y},
                Velocity_t{0.f, 0.f},
                PlayerController_t{playerId, false},
                createDrawable(textureId, {0, 0, 64, 64}, 10, true, 1, 0.f),
                Collider_t{64.f, 64.f, true, 1, 50},
                Health_t{200, 200},
                SendUpdate_t{true});
        return player;
    }

    Entity createProjectile(ECS& ecs, float x, float y, float velocityX, float velocityY,
                            uint8_t team, int damage, const std::string& textureId)
    {
        Entity bullet = ecs.createEntity();

        ecs.addComponents
            <Position_t, Velocity_t, Drawable_t, Collider_t, Projectile_t>
            (bullet,
                Position_t{x, y},
                Velocity_t{velocityX, velocityY},
                createDrawable(textureId, {0, 0, 16, 8}, 20, true, 1.f, 0.f),
                Collider_t{16.f, 8.f, false, team, damage},
                Projectile_t{velocityX, damage});
                // SendUpdate_t{true});
        return bullet;
    }

    Entity createEnemy(ECS& ecs, float x, float y, int health,
                       MovementPattern_t::Type pattern, const std::string& textureId)
    {
        Entity enemy = ecs.createEntity();

        ecs.addComponents<Position_t, Velocity_t, Drawable_t, Collider_t, Health_t, MovementPattern_t, SendUpdate_t>
            (enemy,
                Position_t{x, y},
                Velocity_t{-100.f, 0.f},
                createDrawable(textureId, {0, 0, 64, 64}, 10, true, 1.f, 0.f),
                Collider_t{64.f, 64.f, true, 2, 30},
                Health_t{health, health},
                MovementPattern_t{pattern, 50.f, 2.f, 0.f, 100.f},
                SendUpdate_t{true});
        return enemy;
    }

}