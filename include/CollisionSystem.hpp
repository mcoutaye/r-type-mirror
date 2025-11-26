// ============================================
// 5. COLLISION SYSTEM (AABB brute-force, OK pour 200 entités)
// ============================================

// CollisionSystem.hpp
#pragma once
#include "ecs.hpp"

class CollisionSystem : public ISystem {
public:
    CollisionSystem(ECS& ecs);
    void update(double dt) override;
};