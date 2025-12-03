/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Collision System
*/

#pragma once
#include "engine/ecs/ecs.hpp"
#include "engine/systems/Components.hpp"

class CollisionSystem : public ISystem {
public:
    CollisionSystem(ECS& ecs);
    void update(double dt) override;
};