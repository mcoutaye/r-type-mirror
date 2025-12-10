/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Collision System
*/

#pragma once
#include "engine/ecs/ecs.hpp"
#include "engine/systems/Components.hpp"
#include <algorithm>

class CollisionSystem : public ISystem {
public:
    CollisionSystem(ECS& ecs);
    void update(double dt) override;
    
private:
    // Vérifie si deux rectangles AABB se chevauchent
    bool checkAABB(const Position& pos1, const Collider& coll1,
                   const Position& pos2, const Collider& coll2) const;
    
    // Résout la collision en repoussant l'entité mobile
    void resolveCollision(Position& mobilePos, const Collider& mobileColl,
                          const Position& staticPos, const Collider& staticColl) const;
};