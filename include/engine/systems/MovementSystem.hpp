/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Movement System
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include <cmath>

class MovementSystem : public ISystem {
public:
    MovementSystem(ECS& ecs) : ISystem(ecs) {}
    void update(double dt) override;
};

