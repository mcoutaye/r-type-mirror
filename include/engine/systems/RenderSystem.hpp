/*
** EPITECH PROJECT, 2025
** void RenderSystem::update(double dt)
{
    (void)dt;  // dt non utilisé ici

    // NOTE: clear() et setView() sont gérés par le client avant d'appeler update()

    // === 1. Dessin des étoiles (parallaxe starfield) === File description:
** Render System
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include "engine/systems/RessourceManager.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdlib>


class RenderSystem : public ISystem {
    public:
        RenderSystem(ECS& ecs, sf::RenderWindow& window, ResourceManager& resourceManager);
        void update(double dt) override;
        void clear(sf::Color color = sf::Color::Black) { _window.clear(color); }
        void debugON() { debugMode = true; }
        void debugOFF() { debugMode = false; }

    private:
        void debugColliders();
        bool debugMode = false;
        sf::RenderWindow& _window;
        ResourceManager &_resourceManager;
};

RenderSystem::RenderSystem(ECS& ecs, sf::RenderWindow& window, ResourceManager& resourceManager)
    : ISystem(ecs), _window(window), _resourceManager(resourceManager)
{
}

void RenderSystem::update(double dt)
{
    // NOTE: clear() and setView() are handled by the client before calling update()

    // === 1. Dessin des étoiles (parallaxe starfield) ===
    std::vector<Entity> stars = _ecs.getEntitiesByComponents<Star_t, Position_t>();
    for (Entity e : stars) {
        auto* pos = _ecs.getComponent<Position_t>(e);
        auto* star = _ecs.getComponent<Star_t>(e);

        // Don't recycle stars here - let Client::update() handle star creation/destruction
        // This prevents entity count from growing unbounded
        if (!pos || !star) continue;

        // Réapparition à droite quand l'étoile sort à gauche
        // if (pos->x < -10.f) {
        //     pos->x = 1930.f;
        //     pos->y = static_cast<float>(rand() % 1080);
        // }

        sf::RectangleShape shape(sf::Vector2f(star->size, star->size));
        shape.setPosition(pos->x, pos->y);
        shape.setFillColor(sf::Color(255, 255, 255, star->brightness));
        _window.draw(shape);
    }

    // ============================================================
    // ENTITY RENDERING WITH ANIMATION SUPPORT
    // ============================================================
    std::vector<Entity> entities = _ecs.getEntitiesByComponents<Position_t, Drawable_t>();

    // Sort entities by layer
    std::sort(entities.begin(), entities.end(), [this](Entity a, Entity b) {
        auto* drawableA = _ecs.getComponent<Drawable_t>(a);
        auto* drawableB = _ecs.getComponent<Drawable_t>(b);
        return drawableA->layer < drawableB->layer;
    });

    for (Entity e : entities) {
        auto* pos = _ecs.getComponent<Position_t>(e);
        auto* drawable = _ecs.getComponent<Drawable_t>(e);

        if (!pos || !drawable || !drawable->visible || drawable->frames.empty())
            continue;

        // Update animation
        drawable->frameTimer += dt;
        if (drawable->frameTimer >= drawable->animationSpeed) {
            drawable->frameTimer = 0.0f;
            drawable->currentFrameIndex++;

            if (drawable->currentFrameIndex >= drawable->frames.size()) {
                if (drawable->loop) {
                    drawable->currentFrameIndex = 0;
                } else {
                    drawable->currentFrameIndex = drawable->frames.size() - 1;
                }
            }
        }

        // Render current frame (centered origin to match center-based colliders)
        sf::Sprite sprite;
        sprite.setTexture(_resourceManager.getTexture(drawable->textureName));
        sf::IntRect frame = drawable->frames[drawable->currentFrameIndex];
        sprite.setTextureRect(frame);
        sprite.setOrigin(frame.width / 2.f, frame.height / 2.f);
        sprite.setPosition(pos->x, pos->y);
        sprite.setScale(drawable->scale, drawable->scale);
        sprite.setRotation(drawable->rotation);

        _window.draw(sprite);
    }

    // === 3. Dessin du texte (menu, UI, etc.) ===
    auto textEntities = _ecs.getEntitiesByComponents<Position_t, Text_t>();
    for (Entity e : textEntities) {
        auto* pos = _ecs.getComponent<Position_t>(e);
        auto* textComp = _ecs.getComponent<Text_t>(e);
        if (!pos || !textComp || !textComp->visible) continue;

        // Utilise la police spécifiée dans textComp->fontId
        const sf::Font& font = _resourceManager.getFont(textComp->fontId);

        sf::Text text;
        text.setFont(font);
        text.setString(textComp->text);
        text.setCharacterSize(textComp->fontSize);
        text.setFillColor(textComp->color);

        // Centrage horizontal si demandé
        sf::FloatRect bounds = text.getLocalBounds();
        float x = pos->x;
        if (textComp->centered) {
            x -= (bounds.width / 2.f) + bounds.left;
        }
        float y = pos->y - (bounds.height / 2.f) - bounds.top;
        text.setPosition(x, y);
        _window.draw(text);
    }

        if (debugMode)
            debugColliders();

    // Display everything
    _window.display();
}

void RenderSystem::debugColliders()
{
    std::vector<Entity> entities = _ecs.getEntitiesByComponents<Position_t, Collider_t>();

    for (Entity e : entities) {
        auto* pos = _ecs.getComponent<Position_t>(e);
        auto* col = _ecs.getComponent<Collider_t>(e);

        if (!pos || !col)
            continue;

        sf::RectangleShape rectangle(sf::Vector2f(col->width, col->height));
        // Center-based debug collider to match collision system
        // sf::RectangleShape uses top-left positioning, so offset by half size
        rectangle.setPosition(pos->x - col->width / 2.f, pos->y - col->height / 2.f);
        rectangle.setFillColor(sf::Color(0, 0, 0, 0));
        rectangle.setOutlineColor(sf::Color::Red);
        rectangle.setOutlineThickness(1.f);

        _window.draw(rectangle);
    }
}
