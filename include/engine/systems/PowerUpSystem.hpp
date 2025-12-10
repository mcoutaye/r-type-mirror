#pragma once
#include "engine/ecs/ecs.hpp"
#include "Components.hpp"
#include <iostream>

class PowerUpSystem : public ISystem {
public:
    PowerUpSystem(ECS& ecs) : ISystem(ecs) {}
    void update(double dt) override;
};