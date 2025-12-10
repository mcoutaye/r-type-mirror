/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Stage Factory
*/

#include "engine/systems/StageFactory.hpp"

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
            ecs.addComponent(star, Position{x, y});
            ecs.addComponent(star, Velocity{-speed, 0.f});
            ecs.addComponent(star, Star{speed, brightness, size});
        }
    }

    Entity createObstacle(ECS& ecs, float x, float y, float width, float height)
    {
        Entity obstacle = ecs.createEntity();
        ecs.addComponent(obstacle, Position{x, y});
        ecs.addComponent(obstacle, Collider{width, height, true, 0, 0});
        ecs.addComponent(obstacle, Obstacle{true});
        return obstacle;
    }

    void createScreenBorders(ECS& ecs, float screenWidth, float screenHeight, float wallThickness)
    {
        createObstacle(ecs, 0.f, 0.f, screenWidth, wallThickness);
        createObstacle(ecs, 0.f, screenHeight - wallThickness, screenWidth, wallThickness);
    }

    Entity createDestructibleTile(ECS& ecs, float x, float y, float width, float height, int health)
    {
        Entity tile = ecs.createEntity();
        ecs.addComponent(tile, Position{x, y});
        ecs.addComponent(tile, Collider{width, height, true, 0, 0});
        ecs.addComponent(tile, DestructibleTile{health});
        ecs.addComponent(tile, Health{health, health});
        return tile;
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
