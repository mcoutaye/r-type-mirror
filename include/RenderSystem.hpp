// ============================================
// 3. RENDER SYSTEM
// ============================================

// RenderSystem.hpp
#pragma once
#include "ecs.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>


class RenderSystem : public ISystem {
public:
    RenderSystem(ECS& ecs, sf::RenderWindow& window);
    void update(double dt) override;

private:
    sf::RenderWindow& _window;
};