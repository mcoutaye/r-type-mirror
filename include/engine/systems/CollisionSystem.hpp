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
    bool checkAABB(const Components::Position& pos1, const Components::Collider& coll1,
                   const Components::Position& pos2, const Components::Collider& coll2) const;
    
    // Résout la collision en repoussant l'entité mobile
    void resolveCollision(Components::Position& mobilePos, const Components::Collider& mobileColl,
                          const Components::Position& staticPos, const Components::Collider& staticColl) const;
};