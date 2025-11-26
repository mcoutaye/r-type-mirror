// ============================================
// 4. INPUT SYSTEM (clavier simple)
// ============================================

// InputSystem.hpp
#pragma once
#include "ecs.hpp"

class InputSystem : public ISystem {
public:
    InputSystem(ECS& ecs);
    void update(double dt) override;

private:
    float _shootCooldown[4] = {0};  // un par playerId
};