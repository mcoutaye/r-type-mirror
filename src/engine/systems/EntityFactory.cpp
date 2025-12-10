/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Entity Factory
*/

#include "engine/systems/EntityFactory.hpp"

namespace Factory {
    Entity createPlayer(ECS& ecs, float x, float y, uint8_t playerId, const std::string& textureId)
    {
        Entity player = ecs.createEntity();
        ecs.addComponent(player, Position{x, y});
        ecs.addComponent(player, Velocity{0.f, 0.f});
        ecs.addComponent(player, PlayerController{playerId, false});
        ecs.addComponent(player, Drawable{textureId, {0, 0, 64, 64}, 10, true});
        ecs.addComponent(player, Collider{64.f, 64.f, true, 1, 50});
        ecs.addComponent(player, Health{200, 200});
        return player;
    }

    Entity createProjectile(ECS& ecs, float x, float y, float velocityX, float velocityY,
                            uint8_t team, int damage, const std::string& textureId)
    {
        Entity bullet = ecs.createEntity();
        ecs.addComponent(bullet, Position{x, y});
        ecs.addComponent(bullet, Velocity{velocityX, velocityY});
        ecs.addComponent(bullet, Drawable{textureId, {0, 0, 16, 8}, 20, true, 1.f, 0.f});
        ecs.addComponent(bullet, Collider{16.f, 8.f, false, team, damage});
        ecs.addComponent(bullet, Projectile{velocityX, damage});
        return bullet;
    }

    Entity createEnemy(ECS& ecs, float x, float y, int health,
                       MovementPattern::Type pattern, const std::string& textureId)
    {
        Entity enemy = ecs.createEntity();
        ecs.addComponent(enemy, Position{x, y});
        ecs.addComponent(enemy, Velocity{-100.f, 0.f});
        ecs.addComponent(enemy, Drawable{textureId, {0, 0, 64, 64}, 10, true});
        ecs.addComponent(enemy, Collider{64.f, 64.f, true, 2, 30});
        ecs.addComponent(enemy, Health{health, health});
        ecs.addComponent(enemy, MovementPattern{pattern, 50.f, 2.f, 0.f, 100.f});
        return enemy;
    }
}
