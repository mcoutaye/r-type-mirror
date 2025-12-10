/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Stage Factory
*/

#pragma once
#include "engine/ecs/ecs.hpp"
#include "engine/systems/Components.hpp"
#include <vector>
#include <cstdlib>

namespace Factory {
    void createStarfield(ECS& ecs, int numStars, float screenWidth, float screenHeight, int layers = 3); // background
    Entity createObstacle(ECS& ecs, float x, float y, float width, float height); 
    void createScreenBorders(ECS& ecs, float screenWidth, float screenHeight, float wallThickness = 20.f);
    Entity createDestructibleTile(ECS& ecs, float x, float y, float width = 50.f, float height = 50.f, int health = 50);
    std::vector<Entity> createTileGrid(ECS& ecs, float startX, float startY, int cols, int rows,
                                        float tileWidth = 50.f, float tileHeight = 50.f, int health = 50);
}
