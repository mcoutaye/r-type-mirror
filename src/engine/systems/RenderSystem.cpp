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
    _window.clear(sf::Color(10, 10, 30));

    // Stars (background)
    for (Entity e : _ecs.getEntitiesByComponents<Position, Star>()) {
        auto* pos = _ecs.getComponent<Position>(e);
        auto* star = _ecs.getComponent<Star>(e);
        if (!pos || !star) continue;
        sf::CircleShape shape(star->size);
        shape.setFillColor(sf::Color(star->brightness, star->brightness, star->brightness));
        shape.setPosition(pos->x, pos->y);
        _window.draw(shape);
    }

    // Obstacles
    for (Entity e : _ecs.getEntitiesByComponents<Position, Collider, Obstacle>()) {
        auto* pos = _ecs.getComponent<Position>(e);
        auto* coll = _ecs.getComponent<Collider>(e);
        if (!pos || !coll) continue;
        sf::RectangleShape shape(sf::Vector2f(coll->width, coll->height));
        shape.setFillColor(sf::Color(60, 60, 70));
        shape.setOutlineColor(sf::Color(100, 100, 120));
        shape.setOutlineThickness(2.f);
        shape.setPosition(pos->x, pos->y);
        _window.draw(shape);
    }

    // Destructible tiles
    for (Entity e : _ecs.getEntitiesByComponents<Position, Collider, DestructibleTile>()) {
        auto* pos = _ecs.getComponent<Position>(e);
        auto* coll = _ecs.getComponent<Collider>(e);
        auto* health = _ecs.getComponent<Health>(e);
        if (!pos || !coll) continue;
        float ratio = health ? (float)health->current / health->max : 1.f;
        sf::RectangleShape shape(sf::Vector2f(coll->width, coll->height));
        shape.setFillColor(sf::Color(180 + (1.f - ratio) * 75, 100 * ratio, 50));
        shape.setOutlineColor(sf::Color(139, 69, 19));
        shape.setOutlineThickness(2.f);
        shape.setPosition(pos->x, pos->y);
        _window.draw(shape);
    }

    // Drawable entities (sprites) - only if not handled elsewhere
    auto entities = _ecs.getEntitiesByComponents<Position, Drawable>();
    std::vector<std::pair<int, Entity>> sorted;
    for (Entity e : entities) {
        // Skip entities already drawn as shapes
        if (_ecs.hasComponent<Star>(e)) continue;
        if (_ecs.hasComponent<Obstacle>(e)) continue;
        if (_ecs.hasComponent<DestructibleTile>(e)) continue;
        if (_ecs.hasComponent<Projectile>(e)) continue;
        if (_ecs.hasComponent<PlayerController>(e)) continue;
        if (_ecs.hasComponent<Health>(e) && !_ecs.hasComponent<PlayerController>(e)) continue; // Enemies
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

    // Projectiles
    for (Entity e : _ecs.getEntitiesByComponents<Position, Projectile, Collider>()) {
        auto* pos = _ecs.getComponent<Position>(e);
        auto* coll = _ecs.getComponent<Collider>(e);
        if (!pos || !coll) continue;
        sf::RectangleShape shape(sf::Vector2f(coll->width, coll->height));
        shape.setFillColor((coll->team == 1) ? sf::Color::Cyan : sf::Color(255, 100, 100));
        shape.setPosition(pos->x, pos->y);
        _window.draw(shape);
    }

    // Player
    for (Entity e : _ecs.getEntitiesByComponents<Position, Collider, PlayerController>()) {
        auto* pos = _ecs.getComponent<Position>(e);
        auto* coll = _ecs.getComponent<Collider>(e);
        if (!pos || !coll) continue;
        sf::RectangleShape shape(sf::Vector2f(coll->width, coll->height));
        shape.setFillColor(sf::Color(50, 150, 255));
        shape.setOutlineColor(sf::Color::White);
        shape.setOutlineThickness(2.f);
        shape.setPosition(pos->x, pos->y);
        _window.draw(shape);
    }

    // Enemies
    for (Entity e : _ecs.getEntitiesByComponents<Position, Collider, Health>()) {
        if (_ecs.hasComponent<PlayerController>(e)) continue;
        if (_ecs.hasComponent<DestructibleTile>(e)) continue;
        if (_ecs.hasComponent<Projectile>(e)) continue;
        if (_ecs.hasComponent<Obstacle>(e)) continue;
        auto* pos = _ecs.getComponent<Position>(e);
        auto* coll = _ecs.getComponent<Collider>(e);
        if (!pos || !coll) continue;
        sf::RectangleShape shape(sf::Vector2f(coll->width, coll->height));
        shape.setFillColor(sf::Color(200, 50, 50));
        shape.setOutlineColor(sf::Color(255, 100, 100));
        shape.setOutlineThickness(2.f);
        shape.setPosition(pos->x, pos->y);
        _window.draw(shape);
    }

    _window.display();
}