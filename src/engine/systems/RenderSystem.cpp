/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Render System
*/

#include "engine/systems/RenderSystem.hpp"
#include "engine/systems/RessourceManager.hpp"

RenderSystem::RenderSystem(ECS& ecs, sf::RenderWindow& window)
    : ISystem(ecs), _window(window) {}

void RenderSystem::update(double dt)
{
    _window.clear(sf::Color::Black);   // OBLIGATOIRE EN PREMIER

    auto entities = _ecs.getEntitiesByComponents<Position, Drawable>();
    std::vector<std::pair<int, Entity>> sorted;
    for (Entity e : entities) {
        if (auto* d = _ecs.getComponent<Drawable>(e))
            sorted.emplace_back(d->layer, e);
    }
    std::sort(sorted.begin(), sorted.end());

    for (auto& [layer, e] : sorted) {
        auto* pos = _ecs.getComponent<Position>(e);
        auto* draw = _ecs.getComponent<Drawable>(e);
        if (!draw || !draw->visible) continue;

        sf::Sprite sprite(ResourceManager::getInstance().getTexture(draw->textureId));
        sprite.setTextureRect(draw->rect);
        sprite.setPosition(pos->x, pos->y);
        sprite.setScale(draw->scale, draw->scale);
        sprite.setRotation(draw->rotation);
        _window.draw(sprite);
    }

    _window.display();   // EN DERNIER
}