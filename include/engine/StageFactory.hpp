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

    void createStarfield(ECS& ecs, int numStars, float screenWidth, float screenHeight, int layers)
    {
        for (int i = 0; i < numStars; ++i) {
            Entity star = ecs.createEntity();
            float x = static_cast<float>(rand() % static_cast<int>(screenWidth));
            float y = static_cast<float>(rand() % static_cast<int>(screenHeight));
            int layer = rand() % layers;
            float speed = 50.f + layer * 75.f;
            uint8_t brightness = static_cast<uint8_t>(80 + layer * (170 / layers));
            uint8_t size = static_cast<uint8_t>(1 + layer);

            ecs.addComponents<Position_t, Velocity_t, Star_t>
                (star,
                    Position_t{x, y},
                    Velocity_t{-speed, 0.f},
                    Star_t{speed, brightness, size});
        }
    }

    Entity createObstacle(ECS& ecs, float x, float y, float width, float height)
    {
        Entity obstacle = ecs.createEntity();

        ecs.addComponents<Position_t, Collider_t, Obstacle_t, SendUpdate_t>
            (obstacle,
                Position_t{x, y},
                Collider_t{width, height, true, 0, 0},
                Obstacle_t{true},
                SendUpdate_t{true});
        return obstacle;
    }

    Entity createDestructibleTile(ECS& ecs, float x, float y, float width, float height, int health)
    {
        Entity tile = ecs.createEntity();

        ecs.addComponents<Position_t, Collider_t, Health_t, SendUpdate_t>
            (tile,
                Position_t{x, y},
                Collider_t{width, height, true, 0, 0},
                Health_t{health, health},
                SendUpdate_t{true});
        return tile;
    }

    void createScreenBorders(ECS& ecs, float screenWidth, float screenHeight, float wallThickness)
    {
        createDestructibleTile(ecs, 0.f, 0.f, screenWidth, wallThickness, 9999);
        createDestructibleTile(ecs, 0.f, screenHeight - wallThickness, screenWidth, wallThickness, 9999);
    }

    std::vector<Entity> createTileGrid(ECS& ecs, float startX, float startY,
                                        int cols, int rows,
                                        float tileWidth, float tileHeight, int health)
    {
        std::vector<Entity> tiles;
        tiles.reserve(cols * rows);
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                float x = startX + col * tileWidth;
                float y = startY + row * tileHeight;
                tiles.push_back(createDestructibleTile(ecs, x, y, tileWidth, tileHeight, health));
            }
        }
        return tiles;
    }
}