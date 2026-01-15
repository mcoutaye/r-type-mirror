/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Entity Factory
*/
#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include "engine/Menu.hpp"
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

Entity createTextEntity(ECS& ecs, const std::string& text, float x, float y,
                                const std::string& fontId = "default", uint32_t fontSize = 32,
                                sf::Color color = sf::Color::White, bool centered = false)
{
    Entity textEntity = ecs.createEntity();
    ecs.addComponents<Position_t, Text_t>(
        textEntity,
        Position_t{x, y},
        createText(text, fontSize, color, centered, fontId)
    );
    return textEntity;
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

Entity createMenuItem(ECS& ecs, const MenuItem& item)
{
    Entity menuItem = ecs.createEntity();
    ecs.addComponents<Position_t, Text_t, MenuItem_t, Highlight_t>(
        menuItem,
        Position_t{item.xPosition, item.yPosition},
        createText(item.text, item.fontSize, item.color, item.centered, item.fontId, item.visible),
        MenuItem_t{item.action, false},
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

} // namespace Factory