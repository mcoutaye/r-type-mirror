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
#include <cstdlib>


class RenderSystem : public ISystem {
public:
    RenderSystem(ECS& ecs, sf::RenderWindow& window, ResourceManager& resourceManager);
    void update(double dt) override;

private:
    sf::RenderWindow& _window;
    ResourceManager &_resourceManager = ResourceManager::getInstance();
};

RenderSystem::RenderSystem(ECS& ecs, sf::RenderWindow& window, ResourceManager& resourceManager)
    : ISystem(ecs), _window(window), _resourceManager(resourceManager)
{
}

void RenderSystem::update(double dt)
{
    (void)dt;  // dt non utilisé ici

    _window.clear(sf::Color::Black);

    // === 1. Dessin des étoiles (parallaxe starfield) ===
    std::vector<Entity> stars = _ecs.getEntitiesByComponents<Star_t, Position_t>();
    for (Entity e : stars) {
        auto* pos = _ecs.getComponent<Position_t>(e);
        auto* star = _ecs.getComponent<Star_t>(e);
        if (!pos || !star) continue;

        // Réapparition à droite quand l'étoile sort à gauche
        if (pos->x < -10.f) {
            pos->x = 1930.f;
            pos->y = static_cast<float>(rand() % 1080);
        }

        sf::RectangleShape shape(sf::Vector2f(star->size, star->size));
        shape.setPosition(pos->x, pos->y);
        shape.setFillColor(sf::Color(255, 255, 255, star->brightness));
        _window.draw(shape);
    }

    // === 2. Dessin des sprites (triés par layer) ===
    auto spriteEntities = _ecs.getEntitiesByComponents<Position_t, Drawable_t>();
    std::vector<std::pair<int, Entity>> sortedSprites;
    sortedSprites.reserve(spriteEntities.size());

    for (Entity e : spriteEntities) {
        if (auto* d = _ecs.getComponent<Drawable_t>(e)) {
            sortedSprites.emplace_back(d->layer, e);
        }
    }

    std::sort(sortedSprites.begin(), sortedSprites.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;  // Tri croissant par layer
    });

    for (const auto& [layer, e] : sortedSprites) {
        auto* pos = _ecs.getComponent<Position_t>(e);
        auto* draw = _ecs.getComponent<Drawable_t>(e);
        if (!pos || !draw || !draw->visible) continue;

        // Récupération de la texture (gérée par ResourceManager)
        sf::Sprite sprite(_resourceManager.getTexture(draw->textureId));
        sprite.setTextureRect(draw->rect);
        sprite.setPosition(pos->x, pos->y);
        sprite.setScale(draw->scale, draw->scale);
        sprite.setRotation(draw->rotation);
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

    // === Fin du rendu ===
    _window.display();
}