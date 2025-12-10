#pragma once
#include "../ecs/ecs.hpp"
#include "Components.hpp"
#include <iostream>

class MissileSystem : public ISystem {
public:
    MissileSystem(ECS& ecs) : ISystem(ecs) {}
    void update(double dt) override;
};
