// ============================================
// 3. RENDER SYSTEM
// ============================================

// RenderSystem.hpp
#pragma once
#include "ecs.hpp"

class RenderSystem : public ISystem {
public:
    RenderSystem(ECS& ecs, sf::RenderWindow& window);
    void update(double dt) override;

private:
    sf::RenderWindow& _window;
};