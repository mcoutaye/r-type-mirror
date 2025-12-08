/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Render System
*/

#pragma once
#include "engine/ecs/ecs.hpp"
#include "engine/systems/Components.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>


class RenderSystem : public ISystem {
public:
    RenderSystem(ECS& ecs, sf::RenderWindow& window);
    void update(double dt) override;

private:
    sf::RenderWindow& _window;
};