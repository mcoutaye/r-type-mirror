/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Collision System
*/

#include "engine/systems/CollisionSystem.hpp"
#include <cmath>

CollisionSystem::CollisionSystem(ECS& ecs) : ISystem(ecs) {}

bool CollisionSystem::checkAABB(const Position& pos1, const Collider& coll1,
                                 const Position& pos2, const Collider& coll2) const
{
    return pos1.x < pos2.x + coll2.width &&
           pos1.x + coll1.width > pos2.x &&
           pos1.y < pos2.y + coll2.height &&
           pos1.y + coll1.height > pos2.y;
}

void CollisionSystem::resolveCollision(Position& mobilePos, const Collider& mobileColl,
                                        const Position& staticPos, const Collider& staticColl) const
{
    float overlapLeft = (mobilePos.x + mobileColl.width) - staticPos.x;
    float overlapRight = (staticPos.x + staticColl.width) - mobilePos.x;
    float overlapTop = (mobilePos.y + mobileColl.height) - staticPos.y;
    float overlapBottom = (staticPos.y + staticColl.height) - mobilePos.y;
    float minOverlapX = (overlapLeft < overlapRight) ? -overlapLeft : overlapRight;
    float minOverlapY = (overlapTop < overlapBottom) ? -overlapTop : overlapBottom;
    if (std::abs(minOverlapX) < std::abs(minOverlapY))
        mobilePos.x += minOverlapX;
    else
        mobilePos.y += minOverlapY;
}

void CollisionSystem::update(double dt)
{
    auto entities = _ecs.getEntitiesByComponents<Position, Collider>();
    std::vector<Entity> toDestroy;

    auto obstacles = _ecs.getEntitiesByComponents<Position, Collider, Obstacle>();
    auto players = _ecs.getEntitiesByComponents<Position, Collider, PlayerController>();
    auto tiles = _ecs.getEntitiesByComponents<Position, Collider, DestructibleTile>();
    auto projectiles = _ecs.getEntitiesByComponents<Position, Collider, Projectile>();

    // Player vs Obstacles
    for (Entity player : players) {
        auto* playerPos = _ecs.getComponent<Position>(player);
        auto* playerColl = _ecs.getComponent<Collider>(player);
        if (!playerPos || !playerColl) continue;
        for (Entity obstacle : obstacles) {
            auto* obsPos = _ecs.getComponent<Position>(obstacle);
            auto* obsColl = _ecs.getComponent<Collider>(obstacle);
            auto* obs = _ecs.getComponent<Obstacle>(obstacle);
            if (!obsPos || !obsColl || !obs || !obs->blocking) continue;
            if (checkAABB(*playerPos, *playerColl, *obsPos, *obsColl))
                resolveCollision(*playerPos, *playerColl, *obsPos, *obsColl);
        }
    }

    // Player vs Tiles
    for (Entity player : players) {
        auto* playerPos = _ecs.getComponent<Position>(player);
        auto* playerColl = _ecs.getComponent<Collider>(player);
        if (!playerPos || !playerColl) continue;
        for (Entity tile : tiles) {
            auto* tilePos = _ecs.getComponent<Position>(tile);
            auto* tileColl = _ecs.getComponent<Collider>(tile);
            if (!tilePos || !tileColl) continue;
            if (checkAABB(*playerPos, *playerColl, *tilePos, *tileColl))
                resolveCollision(*playerPos, *playerColl, *tilePos, *tileColl);
        }
    }

    // Projectiles vs Tiles
    for (Entity proj : projectiles) {
        auto* projPos = _ecs.getComponent<Position>(proj);
        auto* projColl = _ecs.getComponent<Collider>(proj);
        auto* projData = _ecs.getComponent<Projectile>(proj);
        if (!projPos || !projColl || !projData) continue;
        for (Entity tile : tiles) {
            auto* tilePos = _ecs.getComponent<Position>(tile);
            auto* tileColl = _ecs.getComponent<Collider>(tile);
            auto* tileHealth = _ecs.getComponent<Health>(tile);
            if (!tilePos || !tileColl || !tileHealth) continue;
            if (checkAABB(*projPos, *projColl, *tilePos, *tileColl)) {
                tileHealth->current -= projData->damage;
                toDestroy.push_back(proj);
                if (tileHealth->current <= 0)
                    toDestroy.push_back(tile);
                break;
            }
        }
    }

    // Projectiles vs Obstacles
    for (Entity proj : projectiles) {
        auto* projPos = _ecs.getComponent<Position>(proj);
        auto* projColl = _ecs.getComponent<Collider>(proj);
        if (!projPos || !projColl) continue;
        for (Entity obstacle : obstacles) {
            auto* obsPos = _ecs.getComponent<Position>(obstacle);
            auto* obsColl = _ecs.getComponent<Collider>(obstacle);
            if (!obsPos || !obsColl) continue;
            if (checkAABB(*projPos, *projColl, *obsPos, *obsColl)) {
                toDestroy.push_back(proj);
                break;
            }
        }
    }

    // Generic collisions (projectiles vs enemies, player vs enemies)
    for (size_t i = 0; i < entities.size(); ++i) {
        Entity e1 = entities[i];
        auto* pos1 = _ecs.getComponent<Position>(e1);
        auto* coll1 = _ecs.getComponent<Collider>(e1);
        if (_ecs.hasComponent<Obstacle>(e1) || _ecs.hasComponent<DestructibleTile>(e1)) continue;

        for (size_t j = i + 1; j < entities.size(); ++j) {
            Entity e2 = entities[j];
            auto* pos2 = _ecs.getComponent<Position>(e2);
            auto* coll2 = _ecs.getComponent<Collider>(e2);
            if (_ecs.hasComponent<Obstacle>(e2) || _ecs.hasComponent<DestructibleTile>(e2)) continue;
            if (!checkAABB(*pos1, *coll1, *pos2, *coll2)) continue;

            auto* proj1 = _ecs.getComponent<Projectile>(e1);
            auto* proj2 = _ecs.getComponent<Projectile>(e2);

            if (proj1) {
                auto* h2 = _ecs.getComponent<Health>(e2);
                if (h2 && coll1->team != coll2->team) {
                    h2->current -= proj1->damage;
                    toDestroy.push_back(e1);
                    if (h2->current <= 0) toDestroy.push_back(e2);
                }
            } else if (proj2) {
                auto* h1 = _ecs.getComponent<Health>(e1);
                if (h1 && coll1->team != coll2->team) {
                    h1->current -= proj2->damage;
                    toDestroy.push_back(e2);
                    if (h1->current <= 0) toDestroy.push_back(e1);
                }
            } else {
                auto* h1 = _ecs.getComponent<Health>(e1);
                auto* h2 = _ecs.getComponent<Health>(e2);
                if (h1 && h2 && coll1->team != coll2->team) {
                    h1->current -= coll2->damage;
                    h2->current -= coll1->damage;
                    if (h1->current <= 0) toDestroy.push_back(e1);
                    if (h2->current <= 0) toDestroy.push_back(e2);
                }
            }
        }
    }

    std::sort(toDestroy.begin(), toDestroy.end());
    toDestroy.erase(std::unique(toDestroy.begin(), toDestroy.end()), toDestroy.end());
    for (Entity e : toDestroy)
        _ecs.killEntity(e);
}
