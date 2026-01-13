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

    void createStarfield(ECS& ecs, int starCount, float screenWidth, float screenHeight, float maxSpeed) {
        for (int i = 0; i < starCount; ++i) {
            Entity star = ecs.createEntity();
            float x = static_cast<float>(rand() % static_cast<int>(screenWidth));
            float y = static_cast<float>(rand() % static_cast<int>(screenHeight));

            // Taille aléatoire (1-3)
            uint8_t size = 1 + (rand() % 5);
            // La vitesse dépend de la taille (plus grosse = plus rapide)
            float speed = (15 - size) * 50.f; // Ex: taille 1 → 150, taille 3 → 50
            uint8_t brightness = 150 + (rand() % 105); // 150-255 (plus visible)

            ecs.addComponents<Position_t, Star_t>(
                star,
                Position_t{x, y},
                Star_t{speed, brightness, size}
            );
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