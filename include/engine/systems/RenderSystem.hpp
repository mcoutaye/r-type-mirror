/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Render System
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include "engine/systems/RessourceManager.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>


class RenderSystem : public ISystem {
public:
    RenderSystem(ECS& ecs, sf::RenderWindow& window, ResourceManager& resourceManager);
    void update(double dt) override;

private:
    sf::RenderWindow& _window;
    ResourceManager &_resourceManager = ResourceManager::getInstance();
};

RenderSystem::RenderSystem(ECS& ecs, sf::RenderWindow& window, ResourceManager& resourceManager)
    : ISystem(ecs), _window(window), _resourceManager(resourceManager)  {}

void RenderSystem::update(double dt)
{
    _window.clear(sf::Color::Black);   // OBLIGATOIRE EN PREMIER

    auto entities = _ecs.getEntitiesByComponents<Position_t, Drawable_t>();
    std::vector<std::pair<int, Entity>> sorted;
    for (Entity e : entities) {
        if (auto* d = _ecs.getComponent<Drawable_t>(e))
            sorted.emplace_back(d->layer, e);
    }
    std::sort(sorted.begin(), sorted.end());

    for (auto& [layer, e] : sorted) {
        auto* pos = _ecs.getComponent<Position_t>(e);
        auto* draw = _ecs.getComponent<Drawable_t>(e);
        if (!draw || !draw->visible) continue;

        sf::Sprite sprite(_resourceManager.getTexture(draw->textureId));
        sprite.setTextureRect(draw->rect);
        sprite.setPosition(pos->x, pos->y);
        sprite.setScale(draw->scale, draw->scale);
        sprite.setRotation(draw->rotation);
        _window.draw(sprite);
    }

    _window.display();   // EN DERNIER
}