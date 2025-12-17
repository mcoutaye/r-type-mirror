/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Entity Factory
*/

#pragma once
#include "engine/ecs/ecs.hpp"
#include "engine/systems/Components.hpp"

namespace Factory {
    Entity createPlayer(ECS& ecs, float x, float y, uint8_t playerId = 0, 
                        const std::string& textureId = "player");
    Entity createProjectile(ECS& ecs, float x, float y, float velocityX, float velocityY,
                            uint8_t team = 1, int damage = 25, const std::string& textureId = "bullet");
    Entity createEnemy(ECS& ecs, float x, float y, int health = 100,
                       Components::MovementPattern::Type pattern = Components::MovementPattern::Type::Linear,
                       const std::string& textureId = "enemy");
}
