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

    // Remove only the stars that are outside the visible area (with a small margin)
    void destroyStarfield(ECS& ecs, float screenWidth, float screenHeight, float margin = 50.f)
    {
        auto stars = ecs.getEntitiesByComponents<Star_t, Position_t>();
        for (Entity e : stars) {
            auto* pos = ecs.getComponent<Position_t>(e);
            if (!pos) continue;

            bool outOfBounds = pos->x < -margin || pos->x > screenWidth + margin ||
                               pos->y < -margin || pos->y > screenHeight + margin;
            if (outOfBounds) {
                ecs.killEntity(e);
            }
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

    // Forward declaration for createTileGrid used by createScreenBorders
    std::vector<Entity> createTileGrid(ECS& ecs, float startX, float startY,
                                       int cols, int rows,
                                       float tileWidth, float tileHeight, int health);

    void createScreenBorders(ECS& ecs, float screenWidth, float screenHeight, float wallThickness)
    {
        constexpr int ABSURD_HEALTH = 999999;

        // Top border
        createTileGrid(ecs, 0, -wallThickness, static_cast<int>(screenWidth / wallThickness), 1, wallThickness, wallThickness, ABSURD_HEALTH);
        // Bottom border
        createTileGrid(ecs, 0, screenHeight, static_cast<int>(screenWidth / wallThickness), 1, wallThickness, wallThickness, ABSURD_HEALTH);
        // Left border
        createTileGrid(ecs, -wallThickness, 0, 1, static_cast<int>(screenHeight / wallThickness), wallThickness, wallThickness, ABSURD_HEALTH);
        // Right border
        createTileGrid(ecs, screenWidth, 0, 1, static_cast<int>(screenHeight / wallThickness), wallThickness, wallThickness, ABSURD_HEALTH);
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
                tiles.push_back(createDestructibleTile(ecs, x, y, health, "block"));
            }
        }
        return tiles;
    }
}