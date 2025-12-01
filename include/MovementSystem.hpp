#pragma once
#include "ecs.hpp"
#include "Components.hpp"

class MovementSystem : public ISystem {
public:
    MovementSystem(ECS& ecs) : ISystem(ecs) {}
    void update(double dt) override;
};