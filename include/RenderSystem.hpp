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

// RenderSystem.cpp
#include "RenderSystem.hpp"
#include "ResourceManager.hpp"
#include <algorithm>

RenderSystem::RenderSystem(ECS& ecs, sf::RenderWindow& window)
    : ISystem(ecs), _window(window) {}

void RenderSystem::update(double dt) {
    _window.clear(sf::Color::Black);

    // Récupère TOUTES les entités drawables
    auto entities = _ecs.getEntitiesByComponents<Position, Drawable>();

    // Tri par layer pour render dans l'ordre (background → foreground)
    std::vector<std::pair<int, Entity>> sorted;
    for (Entity e : entities) {
        auto* draw = _ecs.getComponent<Drawable>(e);
        sorted.emplace_back(draw->layer, e);
    }
    std::sort(sorted.begin(), sorted.end());

    // Draw !
    for (auto& [layer, e] : sorted) {
        auto* pos = _ecs.getComponent<Position>(e);
        auto* draw = _ecs.getComponent<Drawable>(e);

        if (!draw->visible) continue;

        sf::Sprite sprite(ResourceManager::getInstance().getTexture(draw->textureId));
        sprite.setTextureRect(draw->rect);
        sprite.setPosition(pos->x, pos->y);
        sprite.setScale(draw->scale, draw->scale);
        sprite.setRotation(draw->rotation);
        _window.draw(sprite);
    }

    _window.display();
}