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

inline PlaySound_t createSound(const std::string& soundId, float volume = 100.f, float pitch = 1.0f)
{
    PlaySound_t s;
    std::memset(s.soundId, 0, sizeof(s.soundId));
    std::strncpy(s.soundId, soundId.c_str(), sizeof(s.soundId) - 1);
    s.volume = volume;
    s.pitch = pitch;
    return s;
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
                        uint8_t team, int damage, const std::string& textureId,
                        int ownerId = -1, const std::string& shootSound = "shoot.ogg")
{
    Entity bullet = ecs.createEntity();
    ecs.addComponents
        <Position_t, Velocity_t, Drawable_t, Collider_t, Projectile_t, PlaySound_t>
        (bullet,
         Position_t{x, y},
         Velocity_t{velocityX, velocityY},
         createDrawable(textureId, {0, 0, 16, 8}, 20, true, 1.f, 0.f),
         Collider_t{16.f, 8.f, false, team, damage},
         Projectile_t{std::abs(velocityX), damage, ownerId},
         createSound(shootSound, 80.f, 1.0f)); // Son de tir joué immédiatement

    return bullet;
}

Entity createEnemy(ECS& ecs, float x, float y, int health,
                   MovementPattern_t::Type pattern, const std::string& textureId,
                   const std::string& deathSound = "enemy_death.ogg")
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

    // Tu pourras plus tard ajouter un composant "DeathSound_t" si tu veux jouer un son à la mort
    // Pour l'instant, on le gère dans un système de mort (ex: DamageSystem)

    return enemy;
}

// Optionnel : fonction pour jouer un son d'explosion sur une entité existante (ex: mort)
void playDeathSound(ECS& ecs, Entity entity, const std::string& soundId = "enemy_explosion.wav", float volume = 90.f)
{
    if (ecs.hasComponent<PlaySound_t>(entity)) {
        // Évite le double son
        return;
    }
    ecs.addComponent(entity, createSound(soundId, volume));
}

} // namespace Factory