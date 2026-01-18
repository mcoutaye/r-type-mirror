/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Stage Factory
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"

namespace Factory {

    // Helper function to create a single star with consistent parameters
    Entity createStar(ECS& ecs, float x, float y, int layers)
    {
        Entity star = ecs.createEntity();
        int layer = rand() % layers;
        float speed = 50.f + layer * 75.f;
        uint8_t brightness = static_cast<uint8_t>(80 + layer * (170 / layers));
        uint8_t size = static_cast<uint8_t>(1 + layer);

        ecs.addComponents<Position_t, Velocity_t, Star_t>
            (star,
                Position_t{x, y},
                Velocity_t{-speed, 0.f},
                Star_t{speed, brightness, size});
        return star;
    }

    void createStarfield(ECS& ecs, int numStars, float screenWidth, float screenHeight, int layers)
    {
        for (int i = 0; i < numStars; ++i) {
            float x = static_cast<float>(rand() % static_cast<int>(screenWidth));
            float y = static_cast<float>(rand() % static_cast<int>(screenHeight));
            createStar(ecs, x, y, layers);
        }
    }
}