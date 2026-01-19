/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Collision System
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"

class CollisionSystem : public ISystem {
private:
    // Helper: Center-based AABB collision check
    bool checkAABB(Position_t* pos1, Collider_t* col1, 
                   Position_t* pos2, Collider_t* col2) const
    {
        float left1 = pos1->x - col1->width / 2.f;
        float right1 = pos1->x + col1->width / 2.f;
        float top1 = pos1->y - col1->height / 2.f;
        float bottom1 = pos1->y + col1->height / 2.f;

        float left2 = pos2->x - col2->width / 2.f;
        float right2 = pos2->x + col2->width / 2.f;
        float top2 = pos2->y - col2->height / 2.f;
        float bottom2 = pos2->y + col2->height / 2.f;

        return (left1 < right2 && right1 > left2 && 
                top1 < bottom2 && bottom1 > top2);
    }

public:
    CollisionSystem(ECS& ecs);
    void update(double dt) override;
};

CollisionSystem::CollisionSystem(ECS& ecs) : ISystem(ecs) {}

void CollisionSystem::update(double dt)
{
    auto entities = _ecs.getEntitiesByComponents<Position_t, Collider_t>();
    std::vector<Entity> deadEntities;  // Track dead entities to ignore them

    for (Entity entityA : entities) {
        // Skip if already marked dead
        if (std::find(deadEntities.begin(), deadEntities.end(), entityA) != deadEntities.end())
            continue;

        auto* posA = _ecs.getComponent<Position_t>(entityA);
        auto* colA = _ecs.getComponent<Collider_t>(entityA);
        auto* healthA = _ecs.getComponent<Health_t>(entityA);
        auto* tileA = _ecs.getComponent<DestructibleTile_t>(entityA);

        for (Entity entityB : entities) {
            if (entityA == entityB)
                continue;

            // Skip if already marked dead
            if (std::find(deadEntities.begin(), deadEntities.end(), entityB) != deadEntities.end())
                continue;

            auto* posB = _ecs.getComponent<Position_t>(entityB);
            auto* colB = _ecs.getComponent<Collider_t>(entityB);
            auto* healthB = _ecs.getComponent<Health_t>(entityB);
            auto* projectileB = _ecs.getComponent<Projectile_t>(entityB);
            auto* projectileA = _ecs.getComponent<Projectile_t>(entityA);
            auto* tileB = _ecs.getComponent<DestructibleTile_t>(entityB);

            // CENTER-BASED AABB collision detection
            if (!checkAABB(posA, colA, posB, colB))
                continue;

            // Same team collision (skip damage)
            if (colA->team == colB->team && colB->team != 0)
                continue;

            // ============================================================
            // PROJECTILE vs DESTRUCTIBLE TILE collision
            // ============================================================
            if (projectileB && tileA && tileA->blocksMovement) {
                // entityB is a projectile hitting tile entityA
                if (healthA) {
                    healthA->current -= projectileB->damage;
                    healthA->lastAttackerId = projectileB->ownerId;
                }
                deadEntities.push_back(entityB);  // Destroy projectile
                continue;
            }

            if (projectileA && tileB && tileB->blocksMovement) {
                // entityA is a projectile hitting tile entityB
                if (healthB) {
                    healthB->current -= projectileA->damage;
                    healthB->lastAttackerId = projectileA->ownerId;
                }
                deadEntities.push_back(entityA);  // Destroy projectile
                continue;
            }

            // ============================================================
            // PROJECTILE vs ENTITY collision
            // ============================================================
            if (projectileB && !tileA) {
                // entityB is a projectile, entityA is the target (not a tile)
                
                // Check ownership: projectile shouldn't hit the entity that fired it
                if (projectileB->ownerId != -1 && 
                    projectileB->ownerId == static_cast<int>(entityA))
                    continue;

                // Check team: don't hit same team
                if (colA->team == colB->team && colB->team != 0)
                    continue;

                // Apply damage if target has health
                if (healthA) {
                    healthA->current -= projectileB->damage;
                    healthA->lastAttackerId = projectileB->ownerId;
                }

                // Mark projectile for destruction
                deadEntities.push_back(entityB);
                continue;
            }

            if (projectileA && !tileB) {
                // entityA is a projectile, entityB is the target (not a tile)
                
                // Check ownership: projectile shouldn't hit the entity that fired it
                if (projectileA->ownerId != -1 && 
                    projectileA->ownerId == static_cast<int>(entityB))
                    continue;

                // Check team: don't hit same team
                if (colA->team == colB->team && colA->team != 0)
                    continue;

                // Apply damage if target has health
                if (healthB) {
                    healthB->current -= projectileA->damage;
                    healthB->lastAttackerId = projectileA->ownerId;
                }

                // Mark projectile for destruction
                deadEntities.push_back(entityA);
                continue;
            }

            // ============================================================
            // ENTITY vs DESTRUCTIBLE TILE collision (block movement)
            // ============================================================
            // Tiles block all non-projectile entities
            if (tileB && tileB->blocksMovement && !projectileA) {
                // EntityA hit a tile - do nothing, movement will be blocked by MoveSystem
                // No damage applied to tile from contact
                continue;
            }

            if (tileA && tileA->blocksMovement && !projectileB) {
                // EntityB hit a tile - do nothing, movement will be blocked by MoveSystem
                // No damage applied to tile from contact
                continue;
            }

            // ============================================================
            // CONTACT DAMAGE (entity vs entity) with cooldown
            // ============================================================
            if (healthA && healthB && colA->team != colB->team && !tileA && !tileB) {
                // Apply contact damage with cooldown
                if (!healthA->contactCooldown) {
                    healthA->current -= colB->damage;
                    healthA->lastAttackerId = static_cast<int>(entityB);
                    healthA->contactCooldown = 0.5f;  // 0.5 second cooldown
                }
                if (!healthB->contactCooldown) {
                    healthB->current -= colA->damage;
                    healthB->lastAttackerId = static_cast<int>(entityA);
                    healthB->contactCooldown = 0.5f;  // 0.5 second cooldown
                }
            }
        }
    }

    // Mark dead projectiles immediately (visual consistency + no need to broadcast)
    for (Entity e : deadEntities) {
        _ecs.killEntity(e);
    }

    // Update contact damage cooldowns for all entities with health.
    // Do NOT kill entities here if health <= 0:
    // Server must broadcast a death tick first, then clean up after broadcast.
    auto allHealthy = _ecs.getEntitiesByComponents<Health_t>();
    for (Entity e : allHealthy) {
        auto* health = _ecs.getComponent<Health_t>(e);
        if (health->contactCooldown > 0.f) {
            health->contactCooldown -= static_cast<float>(dt);
        }
        // Leave entities with health <= 0 alive until Server::broadcast() sends death tick
        // and performs cleanup to ensure clients receive the death notification.
    }
}
